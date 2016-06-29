#ifndef FFF_PROCESSOR_H
#define FFF_PROCESSOR_H

#include "utils/socket.h"
#include "includes.h"
#include <QDebug>
#include <QSettings>

#define GUI_CMD_REQUEST_MESH 0x01
#define GUI_CMD_SEND_POLYGONS 0x02
#define GUI_CMD_FINISH_OBJECT 0x03

//FusedFilamentFabrication processor.
class fffProcessor
{
private:
    int maxObjectHeight;
    int fileNr;
    double totalFlaient;
    GCodeExport gcode;
    ConfigSettings& config;
    TimeKeeper timeKeeper;

    GCodePathConfig skirtConfig;
    GCodePathConfig inset0Config;
    GCodePathConfig insetXConfig;
    GCodePathConfig fillConfig;
    GCodePathConfig supportConfig;
public:
    fffProcessor(ConfigSettings& config)
    : config(config)
    {
        fileNr = 1;
        maxObjectHeight = 0;
    }
    void clearProcessor()
    {
        fileNr = 1;
    }

    double getTotalChooclate()
    {
        double Vol = (totalFlaient)/271.0;
        return Vol;
    }
    bool setTargetFile(const char* filename)
    {
        QSettings settings("printerConfig.ini",QSettings::IniFormat);
        settings.beginGroup("print");
        QString speed = "M220 S"+settings.value("speed").toString()+"\n";
        QString excluder = "\n";
        QString zvalue = "\n";
        QString temps = "\n";


        int rectA = settings.value("rectA").toInt();
        int rectS = settings.value("rectS").toInt();
        int flimant = settings.value("flimant").toInt();
        int widthE = settings.value("widthE").toInt();


        settings.endGroup();
        config.setSliceConfig(speed.toStdString(),excluder.toStdString(),zvalue.toStdString(),temps.toStdString());
        config.setPrintConfig(rectA,rectS,flimant,widthE);

        gcode.setFilename(filename);
        if (gcode.isOpened())
            gcode.writeComment("Generated with Cura_SteamEngine %s", VERSION);

        return gcode.isOpened();
    }

    bool processFile(const char* input_filename)
    {
        if (!gcode.isOpened())
            return false;

        TimeKeeper timeKeeperTotal;
        SliceDataStorage storage;
        clearProcessor();
        preSetup();
        if (!prepareModel(storage, input_filename))
            return false;

        processSliceData(storage);
        writeGCode(storage);

        finalize();
        logProgress("process", 1, 1);//Report the GUI that a file has been fully processed.
        qDebug()<<QObject::tr("Total time elapsed %1.\n").arg(timeKeeperTotal.restart());

        return true;
    }

    void finalize()
    {
        if (!gcode.isOpened())
            return;
        gcode.finalize(maxObjectHeight, config.moveSpeed, config.endCode.c_str());
        totalFlaient = gcode.getTotalFilamentUsed(0);
    }

private:
    void preSetup()
    {
        skirtConfig.setData(config.printSpeed, config.extrusionWidth, "SKIRT");
        inset0Config.setData(config.inset0Speed, config.extrusionWidth, "WALL-OUTER");
        insetXConfig.setData(config.insetXSpeed, config.extrusionWidth, "WALL-INNER");
        fillConfig.setData(config.infillSpeed, config.extrusionWidth, "FILL");
        supportConfig.setData(config.printSpeed, config.extrusionWidth, "SUPPORT");

        for(unsigned int n=1; n<MAX_EXTRUDERS;n++)
            gcode.setExtruderOffset(n, config.extruderOffset[n].p());
        gcode.setFlavor(config.gcodeFlavor);
        gcode.setRetractionSettings(config.retractionAmount, config.retractionSpeed, config.retractionAmountExtruderSwitch, config.minimalExtrusionBeforeRetraction, config.retractionZHop);
    }

    bool prepareModel(SliceDataStorage& storage, const char* input_filename)
    {
        timeKeeper.restart();
        SimpleModel* model = NULL;
        {
            log("Loading %s from disk...\n", input_filename);
            model = loadModelFromFile(input_filename, config.matrix);
        }
        if (!model)
        {
            logError("Failed to load model: %s\n", input_filename);
            return false;
        }
        log("Loaded from disk in %5.3fs\n", timeKeeper.restart());
        log("Analyzing and optimizing model...\n");
        OptimizedModel* optimizedModel = new OptimizedModel(model, Point3(config.objectPosition.X, config.objectPosition.Y, -config.objectSink));
        for(unsigned int v = 0; v < model->volumes.size(); v++)
        {
            log("  Face counts: %i -> %i %0.1f%%\n", (int)model->volumes[v].faces.size(), (int)optimizedModel->volumes[v].faces.size(), float(optimizedModel->volumes[v].faces.size()) / float(model->volumes[v].faces.size()) * 100);
            log("  Vertex counts: %i -> %i %0.1f%%\n", (int)model->volumes[v].faces.size() * 3, (int)optimizedModel->volumes[v].points.size(), float(optimizedModel->volumes[v].points.size()) / float(model->volumes[v].faces.size() * 3) * 100);
            log("  Size: %f %f %f\n", INT2MM(optimizedModel->modelSize.x), INT2MM(optimizedModel->modelSize.y), INT2MM(optimizedModel->modelSize.z));
            log("  vMin: %f %f %f\n", INT2MM(optimizedModel->vMin.x), INT2MM(optimizedModel->vMin.y), INT2MM(optimizedModel->vMin.z));
            log("  vMax: %f %f %f\n", INT2MM(optimizedModel->vMax.x), INT2MM(optimizedModel->vMax.y), INT2MM(optimizedModel->vMax.z));
            log("  vMin: %f %f %f\n", INT2MM(model->min().x), INT2MM(model->min().y), INT2MM(model->min().z));
            log("  vMax: %f %f %f\n", INT2MM(model->max().x), INT2MM(model->max().y), INT2MM(model->max().z));
            log("  Matrix: %f %f %f\n", config.matrix.m[0][0], config.matrix.m[1][0], config.matrix.m[2][0]);
            log("  Matrix: %f %f %f\n", config.matrix.m[0][1], config.matrix.m[1][1], config.matrix.m[2][1]);
            log("  Matrix: %f %f %f\n", config.matrix.m[0][2], config.matrix.m[1][2], config.matrix.m[2][2]);
            if (INT2MM(optimizedModel->modelSize.x) > 10000.0 || INT2MM(optimizedModel->modelSize.y)  > 10000.0 || INT2MM(optimizedModel->modelSize.z) > 10000.0)
            {
                logError("Object is way to big, CuraEngine bug?");
                exit(1);
            }
        }
        delete model;
        log("Optimize model %5.3fs \n", timeKeeper.restart());
        //om->saveDebugSTL("c:\\models\\output.stl");

        log("Slicing model...\n");
        vector<Slicer*> slicerList;
        for(unsigned int volumeIdx=0; volumeIdx < optimizedModel->volumes.size(); volumeIdx++)
        {
            Slicer* slicer = new Slicer(&optimizedModel->volumes[volumeIdx], config.initialLayerThickness - config.layerThickness / 2, config.layerThickness, config.fixHorrible & FIX_HORRIBLE_KEEP_NONE_CLOSED, config.fixHorrible & FIX_HORRIBLE_EXTENSIVE_STITCHING);
            slicerList.push_back(slicer);
            for(unsigned int layerNr=0; layerNr<slicer->layers.size(); layerNr++)
            {
                //Reporting the outline here slows down the engine quite a bit, so only do so when debugging.
                //sendPolygonsToGui("outline", layerNr, slicer->layers[layerNr].z, slicer->layers[layerNr].polygonList);
//                sendPolygonsToGui("openoutline", layerNr, slicer->layers[layerNr].z, slicer->layers[layerNr].openPolygonList);
            }
        }
        log("Sliced model in %5.3fs\n", timeKeeper.restart());

        log("Generating support map...\n");
        generateSupportGrid(storage.support, optimizedModel, config.supportAngle, config.supportEverywhere > 0, config.supportXYDistance, config.supportZDistance);

        storage.modelSize = optimizedModel->modelSize;
        storage.modelMin = optimizedModel->vMin;
        storage.modelMax = optimizedModel->vMax;
        delete optimizedModel;

        log("Generating layer parts...\n");
        for(unsigned int volumeIdx=0; volumeIdx < slicerList.size(); volumeIdx++)
        {
            storage.volumes.push_back(SliceVolumeStorage());
            createLayerParts(storage.volumes[volumeIdx], slicerList[volumeIdx], config.fixHorrible & (FIX_HORRIBLE_UNION_ALL_TYPE_A | FIX_HORRIBLE_UNION_ALL_TYPE_B | FIX_HORRIBLE_UNION_ALL_TYPE_C));
            delete slicerList[volumeIdx];
            
            //Add the raft offset to each layer.
            for(unsigned int layerNr=0; layerNr<storage.volumes[volumeIdx].layers.size(); layerNr++)
                storage.volumes[volumeIdx].layers[layerNr].printZ += config.raftBaseThickness + config.raftInterfaceThickness;
        }
        log("Generated layer parts in %5.3fs\n", timeKeeper.restart());
        return true;
    }

    void processSliceData(SliceDataStorage& storage)
    {
        //carveMultipleVolumes(storage.volumes);
        generateMultipleVolumesOverlap(storage.volumes, config.multiVolumeOverlap);
        //dumpLayerparts(storage, "c:/models/output.html");

        const unsigned int totalLayers = storage.volumes[0].layers.size();
        for(unsigned int layerNr=0; layerNr<totalLayers; layerNr++)
        {
            for(unsigned int volumeIdx=0; volumeIdx<storage.volumes.size(); volumeIdx++)
            {
                int insetCount = config.insetCount;
                if (config.spiralizeMode && int(layerNr) < config.downSkinCount && layerNr % 2 == 1)//Add extra insets every 2 layers when spiralizing, this makes bottoms of cups watertight.
                    insetCount += 5;
                SliceLayer* layer = &storage.volumes[volumeIdx].layers[layerNr];
                int extrusionWidth = config.extrusionWidth;
                if (layerNr == 0)
                    extrusionWidth = config.layer0extrusionWidth;
                generateInsets(layer, extrusionWidth, insetCount);
            }
            logProgress("inset",layerNr+1,totalLayers);
        }
        if (config.enableOozeShield)
        {
            for(unsigned int layerNr=0; layerNr<totalLayers; layerNr++)
            {
                Polygons oozeShield;
                for(unsigned int volumeIdx=0; volumeIdx<storage.volumes.size(); volumeIdx++)
                {
                    for(unsigned int partNr=0; partNr<storage.volumes[volumeIdx].layers[layerNr].parts.size(); partNr++)
                    {
                        oozeShield = oozeShield.unionPolygons(storage.volumes[volumeIdx].layers[layerNr].parts[partNr].outline.offset(MM2INT(2.0)));
                    }
                }
                storage.oozeShield.push_back(oozeShield);
            }

            for(unsigned int layerNr=0; layerNr<totalLayers; layerNr++)
                storage.oozeShield[layerNr] = storage.oozeShield[layerNr].offset(-MM2INT(1.0)).offset(MM2INT(1.0));
            int offsetAngle = tan(60.0*3.1415/180) * config.layerThickness;//Allow for a 60deg angle in the oozeShield.
            for(unsigned int layerNr=1; layerNr<totalLayers; layerNr++)
                storage.oozeShield[layerNr] = storage.oozeShield[layerNr].unionPolygons(storage.oozeShield[layerNr-1].offset(-offsetAngle));
            for(unsigned int layerNr=totalLayers-1; layerNr>0; layerNr--)
                storage.oozeShield[layerNr-1] = storage.oozeShield[layerNr-1].unionPolygons(storage.oozeShield[layerNr].offset(-offsetAngle));
        }
        log("Generated inset in %5.3fs\n", timeKeeper.restart());

        for(unsigned int layerNr=0; layerNr<totalLayers; layerNr++)
        {
            if (!config.spiralizeMode || int(layerNr) < config.downSkinCount)    //Only generate up/downskin and infill for the first X layers when spiralize is choosen.
            {
                for(unsigned int volumeIdx=0; volumeIdx<storage.volumes.size(); volumeIdx++)
                {
                    int extrusionWidth = config.extrusionWidth;
                    if (layerNr == 0)
                        extrusionWidth = config.layer0extrusionWidth;
                    generateSkins(layerNr, storage.volumes[volumeIdx], extrusionWidth, config.downSkinCount, config.upSkinCount, config.infillOverlap);
                    generateSparse(layerNr, storage.volumes[volumeIdx], extrusionWidth, config.downSkinCount, config.upSkinCount);

//                    SliceLayer* layer = &storage.volumes[volumeIdx].layers[layerNr];

                }
            }
            logProgress("skin",layerNr+1,totalLayers);
        }
        log("Generated up/down skin in %5.3fs\n", timeKeeper.restart());

        if (config.wipeTowerSize > 0)
        {
            PolygonRef p = storage.wipeTower.newPoly();
            p.add(Pointc(storage.modelMin.x - 3000, storage.modelMax.y + 3000));
            p.add(Pointc(storage.modelMin.x - 3000, storage.modelMax.y + 3000 + config.wipeTowerSize));
            p.add(Pointc(storage.modelMin.x - 3000 - config.wipeTowerSize, storage.modelMax.y + 3000 + config.wipeTowerSize));
            p.add(Pointc(storage.modelMin.x - 3000 - config.wipeTowerSize, storage.modelMax.y + 3000));

            storage.wipePoint = Pointc(storage.modelMin.x - 3000 - config.wipeTowerSize / 2, storage.modelMax.y + 3000 + config.wipeTowerSize / 2);
        }

        generateSkirt(storage, config.skirtDistance, config.layer0extrusionWidth, config.skirtLineCount, config.skirtMinLength, config.initialLayerThickness);
        generateRaft(storage, config.raftMargin);

        for(unsigned int volumeIdx=0; volumeIdx<storage.volumes.size(); volumeIdx++)
        {
            for(unsigned int layerNr=0; layerNr<totalLayers; layerNr++)
            {
                for(unsigned int partNr=0; partNr<storage.volumes[volumeIdx].layers[layerNr].parts.size(); partNr++)
                {
                    if (layerNr > 0)
                        storage.volumes[volumeIdx].layers[layerNr].parts[partNr].bridgeAngle = bridgeAngle(&storage.volumes[volumeIdx].layers[layerNr].parts[partNr], &storage.volumes[volumeIdx].layers[layerNr-1]);
                    else
                        storage.volumes[volumeIdx].layers[layerNr].parts[partNr].bridgeAngle = -1;
                }
            }
        }
    }

    void writeGCode(SliceDataStorage& storage)
    {
        if (fileNr == 1)    //判断是否是开头？
        {
            if (gcode.getFlavor() == GCODE_FLAVOR_ULTIGCODE)
            {
                gcode.writeComment("FLAVOR:UltiGCode");
                gcode.writeComment("TIME:<__TIME__>");
                gcode.writeComment("MATERIAL:<FILAMENT>");
                gcode.writeComment("MATERIAL2:<FILAMEN2>");
            }
            gcode.writeCode(config.startCode.c_str());
            if (gcode.getFlavor() == GCODE_FLAVOR_BFB)
            {
                gcode.writeComment("enable auto-retraction");
                gcode.writeLine("M227 S%d P%d", config.retractionAmount * 2560 / 1000, config.retractionAmount * 2560 / 1000);
            }
        }else
        {
            gcode.writeFanCommand(0);
            gcode.resetExtrusionValue();
            gcode.writeRetraction();
            gcode.setZ(maxObjectHeight + 5000);
            gcode.writeMove(gcode.getPositionXY(), config.moveSpeed, 0);
            gcode.writeMove(Pointc(storage.modelMin.x, storage.modelMin.y), config.moveSpeed, 0);
        }
        fileNr++;

        unsigned int totalLayers = storage.volumes[0].layers.size();

        //修改要导出的数量
        totalLayers = 2;
        gcode.writeComment("Layer count: %d", totalLayers);

        if (config.raftBaseThickness > 0 && config.raftInterfaceThickness > 0)
        {   
            GCodePathConfig raftBaseConfig(config.initialLayerSpeed, config.raftBaseLinewidth, "SUPPORT");
            GCodePathConfig raftInterfaceConfig(config.initialLayerSpeed, config.raftInterfaceLinewidth, "SUPPORT");
            {
                gcode.writeComment("LAYER:-2");
                gcode.writeComment("RAFT");
                GCodePlanner gcodeLayer(gcode, config.moveSpeed, config.retractionMinimalDistance);
                if (config.supportExtruder > 0)
                    gcodeLayer.setExtruder(config.supportExtruder);
                gcodeLayer.setAlwaysRetract(true);
                gcode.setZ(config.raftBaseThickness);
                gcode.setExtrusion(config.raftBaseThickness, config.filamentDiameter, config.filamentFlow);
                gcodeLayer.addPolygonsByOptimizer(storage.raftOutline, &raftBaseConfig);

                Polygons raftLines;
                generateLineInfill(storage.raftOutline, raftLines, config.raftBaseLinewidth, config.raftLineSpacing, config.infillOverlap, 0);
                gcodeLayer.addPolygonsByOptimizer(raftLines, &raftBaseConfig);

                gcodeLayer.writeGCode(false, config.raftBaseThickness);
            }

            {
                gcode.writeComment("LAYER:-1");
                gcode.writeComment("RAFT");
                GCodePlanner gcodeLayer(gcode, config.moveSpeed, config.retractionMinimalDistance);
                gcodeLayer.setAlwaysRetract(true);
                gcode.setZ(config.raftBaseThickness + config.raftInterfaceThickness);
                gcode.setExtrusion(config.raftInterfaceThickness, config.filamentDiameter, config.filamentFlow);

                Polygons raftLines;
                generateLineInfill(storage.raftOutline, raftLines, config.raftInterfaceLinewidth, config.raftLineSpacing, config.infillOverlap, 90);
                gcodeLayer.addPolygonsByOptimizer(raftLines, &raftInterfaceConfig);

                gcodeLayer.writeGCode(false, config.raftInterfaceThickness);
            }
        }

        int volumeIdx = 0;
        for(unsigned int layerNr=0; layerNr<totalLayers; layerNr++)
        {
            logProgress("export", layerNr+1, totalLayers);

            int extrusionWidth = config.extrusionWidth;
            if (layerNr == 0)
                extrusionWidth = config.layer0extrusionWidth;
            if (int(layerNr) < config.initialSpeedupLayers)
            {
                int n = config.initialSpeedupLayers;
                skirtConfig.setData(config.printSpeed * layerNr / n + config.initialLayerSpeed * (n - layerNr) / n, extrusionWidth, "SKIRT");
                inset0Config.setData(config.inset0Speed * layerNr / n + config.initialLayerSpeed * (n - layerNr) / n, extrusionWidth, "WALL-OUTER");
                insetXConfig.setData(config.insetXSpeed * layerNr / n + config.initialLayerSpeed * (n - layerNr) / n, extrusionWidth, "WALL-INNER");
                fillConfig.setData(config.infillSpeed * layerNr / n + config.initialLayerSpeed * (n - layerNr) / n, extrusionWidth, "FILL");
                supportConfig.setData(config.printSpeed * layerNr / n + config.initialLayerSpeed * (n - layerNr) / n, extrusionWidth, "SUPPORT");
            }
            else
            {
                skirtConfig.setData(config.printSpeed, extrusionWidth, "SKIRT");
                inset0Config.setData(config.inset0Speed, extrusionWidth, "WALL-OUTER");
                insetXConfig.setData(config.insetXSpeed, extrusionWidth, "WALL-INNER");
                fillConfig.setData(config.infillSpeed, extrusionWidth, "FILL");
                supportConfig.setData(config.printSpeed, extrusionWidth, "SUPPORT");
            }

            gcode.writeComment("LAYER:%d", layerNr);
            if (layerNr == 0)
                gcode.setExtrusion(config.initialLayerThickness, config.filamentDiameter, config.filamentFlow);
            else
                gcode.setExtrusion(config.layerThickness, config.filamentDiameter, config.filamentFlow);


            GCodePlanner gcodeLayer(gcode, config.moveSpeed, config.retractionMinimalDistance);
            int32_t z = config.initialLayerThickness + layerNr * config.layerThickness;
            z += config.raftBaseThickness + config.raftInterfaceThickness;
            gcode.setZ(z);

            bool printSupportFirst = (storage.support.generated && config.supportExtruder > 0 && config.supportExtruder == gcodeLayer.getExtruder());
            if (printSupportFirst)
                addSupportToGCode(storage, gcodeLayer, layerNr);

            for(unsigned int volumeCnt = 0; volumeCnt < storage.volumes.size(); volumeCnt++)
            {
                if (volumeCnt > 0)
                    volumeIdx = (volumeIdx + 1) % storage.volumes.size();
                addVolumeLayerToGCode(storage, gcodeLayer, volumeIdx, layerNr);
            }
            if (!printSupportFirst)
                addSupportToGCode(storage, gcodeLayer, layerNr);

            //Finish the layer by applying speed corrections for minimal layer times
            gcodeLayer.forceMinimalLayerTime(config.minimalLayerTime, config.minimalFeedrate);

            int fanSpeed = config.fanSpeedMin;
            if (gcodeLayer.getExtrudeSpeedFactor() <= 50)
            {
                fanSpeed = config.fanSpeedMax;
            }else{
                int n = gcodeLayer.getExtrudeSpeedFactor() - 50;
                fanSpeed = config.fanSpeedMin * n / 50 + config.fanSpeedMax * (50 - n) / 50;
            }
            if (int(layerNr) < config.fanFullOnLayerNr)
            {
                //Slow down the fan on the layers below the [fanFullOnLayerNr], where layer 0 is speed 0.
                fanSpeed = fanSpeed * layerNr / config.fanFullOnLayerNr;
            }
            gcode.writeFanCommand(fanSpeed);

            gcodeLayer.writeGCode(config.coolHeadLift > 0, int(layerNr) > 0 ? config.layerThickness : config.initialLayerThickness);
        }

        gcode.writeFanCommand(0);

        gcode.tellFileSize();
        log("Wrote layers in %5.2fs.\n", timeKeeper.restart());

        //Store the object height for when we are printing multiple objects, as we need to clear every one of them when moving to the next position.
        maxObjectHeight = std::max(maxObjectHeight, storage.modelSize.z);
    }


    //Add a single layer from a single mesh-volume to the GCode
    void addVolumeLayerToGCode(SliceDataStorage& storage, GCodePlanner& gcodeLayer, int volumeIdx, int layerNr)
    {
        int prevExtruder = gcodeLayer.getExtruder();
        bool extruderChanged = gcodeLayer.setExtruder(volumeIdx);
        if (layerNr == 0 && volumeIdx == 0)
        {
            if (storage.skirt.size() > 0)
                gcodeLayer.addTravel(storage.skirt[storage.skirt.size()-1].closestPointTo(gcode.getPositionXY()));
            gcodeLayer.addPolygonsByOptimizer(storage.skirt, &skirtConfig);
        }

        SliceLayer* layer = &storage.volumes[volumeIdx].layers[layerNr];
        if (extruderChanged)
            addWipeTower(storage, gcodeLayer, layerNr, prevExtruder);

        if (storage.oozeShield.size() > 0 && storage.volumes.size() > 1)
        {
            gcodeLayer.setAlwaysRetract(true);
            gcodeLayer.addPolygonsByOptimizer(storage.oozeShield[layerNr], &skirtConfig);
//            sendPolygonsToGui("oozeshield", layerNr, layer->printZ, storage.oozeShield[layerNr]);
            gcodeLayer.setAlwaysRetract(!config.enableCombing);
        }

        PathOrderOptimizer partOrderOptimizer(gcode.getPositionXY());
        for(unsigned int partNr=0; partNr<layer->parts.size(); partNr++)
        {
            partOrderOptimizer.addPolygon(layer->parts[partNr].insets[0][0]);
        }
        partOrderOptimizer.optimize();

        for(unsigned int partCounter=0; partCounter<partOrderOptimizer.polyOrder.size(); partCounter++)
        {
            SliceLayerPart* part = &layer->parts[partOrderOptimizer.polyOrder[partCounter]];

            if (config.enableCombing)
                gcodeLayer.setCombBoundary(&part->combBoundery);
            else
                gcodeLayer.setAlwaysRetract(true);

            if (config.insetCount > 0)
            {
                if (config.spiralizeMode)
                {
                    if (int(layerNr) >= config.downSkinCount)
                        inset0Config.spiralize = true;
                    if (int(layerNr) == config.downSkinCount && part->insets.size() > 0)
                        gcodeLayer.addPolygonsByOptimizer(part->insets[0], &insetXConfig);
                }
                for(int insetNr=part->insets.size()-1; insetNr>-1; insetNr--)
                {
                    if (insetNr == 0)   //insert0是最外围的一圈，即shell，围墙
                    {
                        gcodeLayer.addPolygonsByOptimizer(part->insets[insetNr], &inset0Config);
                    }
                    else
                        gcodeLayer.addPolygonsByOptimizer(part->insets[insetNr], &insetXConfig);
                }
            }

            Polygons fillPolygons;
            int fillAngle = 45;
            if (layerNr & 1)
                fillAngle += 90;
            int extrusionWidth = config.extrusionWidth;
            if (layerNr == 0)
                extrusionWidth = config.layer0extrusionWidth;
            generateLineInfill(part->skinOutline, fillPolygons, extrusionWidth, extrusionWidth, config.infillOverlap, (part->bridgeAngle > -1) ? part->bridgeAngle : fillAngle);
            if (config.sparseInfillLineDistance > 0)
            {
                if (config.sparseInfillLineDistance > extrusionWidth * 4)
                {
                    generateLineInfill(part->sparseOutline, fillPolygons, extrusionWidth, config.sparseInfillLineDistance * 2, config.infillOverlap, 45);
                    generateLineInfill(part->sparseOutline, fillPolygons, extrusionWidth, config.sparseInfillLineDistance * 2, config.infillOverlap, 45 + 90);
                }
                else
                {
                    generateLineInfill(part->sparseOutline, fillPolygons, extrusionWidth, config.sparseInfillLineDistance, config.infillOverlap, fillAngle);
                }
            }

            gcodeLayer.addPolygonsByOptimizer(fillPolygons, &fillConfig);

            //After a layer part, make sure the nozzle is inside the comb boundary, so we do not retract on the perimeter.
            if (!config.spiralizeMode || int(layerNr) < config.downSkinCount)
                gcodeLayer.moveInsideCombBoundary(config.extrusionWidth * 2);
        }
        gcodeLayer.setCombBoundary(NULL);
    }

    void addSupportToGCode(SliceDataStorage& storage, GCodePlanner& gcodeLayer, int layerNr)
    {
        if (!storage.support.generated)
            return;

        if (config.supportExtruder > -1)
        {
            int prevExtruder = gcodeLayer.getExtruder();
            if (gcodeLayer.setExtruder(config.supportExtruder))
                addWipeTower(storage, gcodeLayer, layerNr, prevExtruder);

            if (storage.oozeShield.size() > 0 && storage.volumes.size() == 1)
            {
                gcodeLayer.setAlwaysRetract(true);
                gcodeLayer.addPolygonsByOptimizer(storage.oozeShield[layerNr], &skirtConfig);
                gcodeLayer.setAlwaysRetract(!config.enableCombing);
            }
        }
        int32_t z = config.initialLayerThickness + layerNr * config.layerThickness;
        SupportPolyGenerator supportGenerator(storage.support, z);
        for(unsigned int volumeCnt = 0; volumeCnt < storage.volumes.size(); volumeCnt++)
        {
            SliceLayer* layer = &storage.volumes[volumeCnt].layers[layerNr];
            for(unsigned int n=0; n<layer->parts.size(); n++)
                supportGenerator.polygons = supportGenerator.polygons.difference(layer->parts[n].outline.offset(config.supportXYDistance));
        }
        //Contract and expand the suppory polygons so small sections are removed and the final polygon is smoothed a bit.
        supportGenerator.polygons = supportGenerator.polygons.offset(-config.extrusionWidth * 3);
        supportGenerator.polygons = supportGenerator.polygons.offset(config.extrusionWidth * 3);
//        sendPolygonsToGui("support", layerNr, z, supportGenerator.polygons);

        vector<Polygons> supportIslands = supportGenerator.polygons.splitIntoParts();

        PathOrderOptimizer islandOrderOptimizer(gcode.getPositionXY());
        for(unsigned int n=0; n<supportIslands.size(); n++)
        {
            islandOrderOptimizer.addPolygon(supportIslands[n][0]);
        }
        islandOrderOptimizer.optimize();

        for(unsigned int n=0; n<supportIslands.size(); n++)
        {
            Polygons& island = supportIslands[islandOrderOptimizer.polyOrder[n]];

            Polygons supportLines;
            if (config.supportLineDistance > 0)
            {
                switch(config.supportType)
                {
                case SUPPORT_TYPE_GRID:
                    if (config.supportLineDistance > config.extrusionWidth * 4)
                    {
                        generateLineInfill(island, supportLines, config.extrusionWidth, config.supportLineDistance*2, config.infillOverlap, 0);
                        generateLineInfill(island, supportLines, config.extrusionWidth, config.supportLineDistance*2, config.infillOverlap, 90);
                    }else{
                        generateLineInfill(island, supportLines, config.extrusionWidth, config.supportLineDistance, config.infillOverlap, (layerNr & 1) ? 0 : 90);
                    }
                    break;
                case SUPPORT_TYPE_LINES:
                    generateLineInfill(island, supportLines, config.extrusionWidth, config.supportLineDistance, config.infillOverlap, 0);
                    break;
                }
            }

            gcodeLayer.forceRetract();
            if (config.enableCombing)
                gcodeLayer.setCombBoundary(&island);
            if (config.supportType == SUPPORT_TYPE_GRID)
                gcodeLayer.addPolygonsByOptimizer(island, &supportConfig);
            gcodeLayer.addPolygonsByOptimizer(supportLines, &supportConfig);
            gcodeLayer.setCombBoundary(NULL);
        }
    }

    void addWipeTower(SliceDataStorage& storage, GCodePlanner& gcodeLayer, int layerNr, int prevExtruder)
    {
        if (config.wipeTowerSize < 1)
            return;
        //If we changed extruder, print the wipe/prime tower for this nozzle;
        gcodeLayer.addPolygonsByOptimizer(storage.wipeTower, &supportConfig);
        Polygons fillPolygons;
        generateLineInfill(storage.wipeTower, fillPolygons, config.extrusionWidth, config.extrusionWidth, config.infillOverlap, 45 + 90 * (layerNr % 2));
        gcodeLayer.addPolygonsByOptimizer(fillPolygons, &supportConfig);

        //Make sure we wipe the old extruder on the wipe tower.
        gcodeLayer.addTravel(storage.wipePoint - config.extruderOffset[prevExtruder].p() + config.extruderOffset[gcodeLayer.getExtruder()].p());
    }


    void carveMultipleVolumes(vector<SliceVolumeStorage> &volumes)
    {
        //Go trough all the volumes, and remove the previous volume outlines from our own outline, so we never have overlapped areas.
        for(unsigned int idx=0; idx < volumes.size(); idx++)
        {
            for(unsigned int idx2=0; idx2<idx; idx2++)
            {
                for(unsigned int layerNr=0; layerNr < volumes[idx].layers.size(); layerNr++)
                {
                    SliceLayer* layer1 = &volumes[idx].layers[layerNr];
                    SliceLayer* layer2 = &volumes[idx2].layers[layerNr];
                    for(unsigned int p1 = 0; p1 < layer1->parts.size(); p1++)
                    {
                        for(unsigned int p2 = 0; p2 < layer2->parts.size(); p2++)
                        {
                            layer1->parts[p1].outline = layer1->parts[p1].outline.difference(layer2->parts[p2].outline);
                        }
                    }
                }
            }
        }
    }

    //Expand each layer a bit and then keep the extra overlapping parts that overlap with other volumes.
    //This generates some overlap in dual extrusion, for better bonding in touching parts.
    void generateMultipleVolumesOverlap(vector<SliceVolumeStorage> &volumes, int overlap)
    {
        if (volumes.size() < 2 || overlap <= 0) return;

        for(unsigned int layerNr=0; layerNr < volumes[0].layers.size(); layerNr++)
        {
            Polygons fullLayer;
            for(unsigned int volIdx = 0; volIdx < volumes.size(); volIdx++)
            {
                SliceLayer* layer1 = &volumes[volIdx].layers[layerNr];
                for(unsigned int p1 = 0; p1 < layer1->parts.size(); p1++)
                {
                    fullLayer = fullLayer.unionPolygons(layer1->parts[p1].outline.offset(20));
                }
            }
            fullLayer = fullLayer.offset(-20);

            for(unsigned int volIdx = 0; volIdx < volumes.size(); volIdx++)
            {
                SliceLayer* layer1 = &volumes[volIdx].layers[layerNr];
                for(unsigned int p1 = 0; p1 < layer1->parts.size(); p1++)
                {
                    layer1->parts[p1].outline = fullLayer.intersection(layer1->parts[p1].outline.offset(overlap / 2));
                }
            }
        }
    }

};

#endif//FFF_PROCESSOR_H
