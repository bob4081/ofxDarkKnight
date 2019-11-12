/*
 Copyright (C) 2018 Luis Fernando Garc�a [http://luiscript.com]
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include "DKModule.hpp"

void DKModule::setupModule(string name, ofVec2f resolution, bool child = false)
{
    moduleIsChild = child;
    setupCommon(name, resolution);
}

void DKModule::setupModule(string name, ofVec2f resolution)
{
    setupCommon(name, resolution);
}

void DKModule::setupCommon(string name, ofVec2f resolution)
{
    moduleGuiWidth = 320;
    moduleName = name;
    moduleId = 0;

	zoom = 1.0;
    
    moduleMidiMapMode = false;
    moduleEnabled = true;
    moduleWidth = resolution.x;
    moduleHeight = resolution.y;

    selectedComponent = nullptr;
    setupGui();
    setup();
    
    moduleIsChild = false;
    moduleInitialized = true;

}

void DKModule::setupGui()
{
    float amp = 1.0;
    
    gui = new ofxDatGui();
    gui->addHeader(moduleName);
    
    if(moduleIsChild) {
        int pixelDensity = ((ofAppGLFWWindow*)ofGetWindowPtr())->getPixelScreenCoordScale();
        amp = pixelDensity >= 2 ? pixelDensity - 0.5 : 1.0;
    }
    addModuleParameters();
    gui->setWidth(moduleGuiWidth * amp);
}

void DKModule::updateModule(float tx, float ty, float zm)
{
	zoom = zm;
	updateModule(tx, ty);
}

void DKModule::updateModule(float tx, float ty)
{
    translation.x = tx;
    translation.y = ty;
    gui->setTranslation(tx, ty, zoom);
    updateModule();
}

void DKModule::updateModule()
{
    gui->update();
    if (moduleEnabled) {
        update();
    }
    
    float x = gui->getPosition().x + gui->getWidth() + 12;
    float y = gui->getPosition().y + 10;

    for (int o=0; o<outputs.size(); o++) {
        outputs[o]->setWireConnectionPos(ofPoint(x,y + o*30));
    }

    x = gui->getPosition().x - 12;
    y = gui->getPosition().y + 10;
    
    for (int i=0; i<inputs.size(); i++) {
        inputs[i]->setWireConnectionPos(ofPoint(x,y + i*30));
    }
    
}

void DKModule::drawModule()
{
    gui->draw();

    if(getModuleMidiMapMode())
    {
        for(auto out : outputs) out->draw();
        for(auto inp : inputs ) inp->draw();
    }
    if (moduleEnabled) draw();
}


void DKModule::toggleMidiMap()
{
    moduleMidiMapMode = !moduleMidiMapMode;
    gui->toggleMidiMode();
}

string DKModule::getName()
{
    return moduleName;
}

int DKModule::getModuleId()
{
    return moduleId;
}
float DKModule::getModuleWidth()
{
    return moduleWidth;
}

float DKModule::getModuleHeight()
{
    return moduleHeight;
}

bool DKModule::getModuleMidiMapMode()
{
    return moduleMidiMapMode;
}

ofxDatGuiComponent * DKModule::getOutputComponent(int x, int y)
{
    return gui->getOutputComponent(x, y);
}

ofxDatGuiComponent * DKModule::getInputComponent(int x, int y)
{
    return gui->getInputComponent(x, y);
}

DKWireConnection * DKModule::getOutputConnection(float x , float y)
{
    DKWireConnection * foundedWireConnection = nullptr;
    for(auto out : outputs)
    {
        foundedWireConnection = out->testWireConnection(x, y);
        if(foundedWireConnection != nullptr)
        {
            return foundedWireConnection;
        }
    }
    return gui->testOutputConnection(x, y);
}

DKWireConnection * DKModule::getInputConnection(float x , float y)
{
    DKWireConnection * foundedWireConnection = nullptr;
    for(auto inp : inputs)
    {
        foundedWireConnection = inp->testWireConnection(x, y);
        if(foundedWireConnection != nullptr)
        {
            return foundedWireConnection;
        }
    }
    return gui->testInputConnection(x, y);
}

ofxDatGuiTheme * DKModule::getGuiTheme()
{
    return nullptr;
}


bool DKModule::getModuleInitialized()
{
    return moduleInitialized;
}

bool DKModule::getModuleEnabled()
{
    return moduleEnabled;
}

bool DKModule::getModuleHasChild()
{
    return moduleHasChild;
}

ofPoint DKModule::getTranslation()
{
    return translation;
}

void DKModule::onSliderEventParent(ofxDatGuiSliderEvent e)
{
    if(e.target->getMidiMode())
    {
        //reset midiMapMode for all components and set current component to midiMapMode
        gui->resetMidiMap();
        e.target->toggleMidiMap(true);
        selectedComponent = e.target;
    }
}

void DKModule::onEnableChange(ofxDatGuiToggleEvent e)
{
    moduleEnabled = e.target->getChecked();
}

void DKModule::setModuleWidth(float w)
{
    moduleWidth = w;
}

void DKModule::setModuleHeight(float h)
{
    moduleHeight = h;
}

void DKModule::setModuleMidiMapMode(bool m)
{
    if(getModuleMidiMapMode() != m)
    {
        moduleMidiMapMode = m;
        gui->toggleMidiMode();
    }
}

void DKModule::setModuleEnabled(bool e)
{
    moduleEnabled = e;
}

void DKModule::setModuleHasChild(bool c)
{
    moduleHasChild = c;
}

void DKModule::setModuleId(int index)
{
    
    moduleId = index;
}

void DKModule::setResolution(int w, int h)
{
    setModuleWidth(w);
    setModuleHeight(h);
}


void DKModule::enable()
{
    gui->setEnabled(true);
    gui->setVisible(true);
    moduleEnabled = true;
}

void DKModule::disable()
{
    gui->setEnabled(false);
    gui->setVisible(false);
    moduleEnabled = false;
}

void DKModule::addSlider(string name, int & add, int min, int max, int val)
{
    gui->addSlider(name, min, max, val)->bind(add);
}

void DKModule::addSlider(string name, float & add, float min, float max, float val)
{
    gui->addSlider(name, min, max, val)->bind(add);
}

void DKModule::addSlider(string name, int & add, int min, int max, int val, int precision)
{
    gui->addSlider(name, min, max, val)->setPrecision(precision)->bind(add);
}

void DKModule::addSlider(string name, float & add, float min, float max, float val, int precision)
{
    gui->addSlider(name, min, max, val)->setPrecision(precision)->bind(add);
}


void DKModule::addInputConnection(DKConnectionType t)
{
    DKWireConnection * input = new DKWireConnection;
    input->setWireConnectionType(t);
    inputs.push_back(input);
}

void DKModule::addOutputConnection(DKConnectionType t)
{
    DKWireConnection * output = new DKWireConnection;
    output->setWireConnectionType(t);
    outputs.push_back(output);
}

void DKModule::addInputConnection(DKConnectionType t, string name)
{
	DKWireConnection* input = new DKWireConnection;
	input->setName(name);
	input->setWireConnectionType(t);
	inputs.push_back(input);
}

void DKModule::addOutputConnection(DKConnectionType t, string name)
{
	DKWireConnection* output = new DKWireConnection;
	output->setName(name);
	output->setWireConnectionType(t);
	outputs.push_back(output);
}

void DKModule::sendMidiMessage(ofxMidiMessage * msg)
{
    outMidiMessages.push_back(msg);
}
