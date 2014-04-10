/**

./inc/scene.hpp

Created on: 16 May 2013

Copyright (c) 2013, Philip Deegan

This file is part of fragex.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _FRAGEX_SCENE_HPP_
#define _FRAGEX_SCENE_HPP_

#include "kul/log.hpp"
#include "kul/string.hpp"

#include "irrlicht.h"

#include "irrlight/scene.hpp"
#include "irrlight/scene.ui.hpp"
#include "irrlight/dev/console.hpp"

using namespace irr;
using namespace irr::gui;

using namespace irrlight;

class Scene;
class SceneKeyHandler;

// TODO IMPLEMENT
class Pixel{};

class Quadrant{
	private:
		uint r,g,b;
		const uint x;
		const uint y;
		const uint w;
		const uint h;
		const std::string i;
		Quadrant(uint x, uint y, uint w, uint h, std::string i) : r(255), g(255), b(255), x(x), y(y), w(w), h(h), i(i){}
		std::vector<Quadrant> solve(){
			std::vector<Quadrant> pixels;
			if(w == 1 && h == 1) pixels.push_back(*this);			
			else{
				for(Quadrant p : Quadrant(x		 , y      , w/2, h/2, i + "0").solve()) pixels.push_back(p);
				for(Quadrant p : Quadrant(x + w/2, y      , w/2, h/2, i + "1").solve()) pixels.push_back(p);
				for(Quadrant p : Quadrant(x      , y + h/2, w/2, h/2, i + "2").solve()) pixels.push_back(p);
				for(Quadrant p : Quadrant(x + w/2, y + h/2, w/2, h/2, i + "3").solve()) pixels.push_back(p);
			}
			return pixels;
		}
		void red  (uint r){ this->r = r; }
		void green(uint g){ this->g = g; }
		void blue (uint b){ this->b = b; }
		void rgb  (uint r, uint g, uint b){ this->r = r; this->g = g; this->b = b; }
	public:		
		friend class Scene;
		friend class SceneKeyHandler;
};

class Scene : public irrlight::AScene {
	private:
		std::vector<Quadrant> pixels;
		irrlight::dev::Console console;
		std::string reg;		
		uint lFps = -1;
	public:
		Scene(){
			const uint w = 512;
			const uint h = 512;

			for(Quadrant p : Quadrant(0  , 0  , w/2, h/2, "0").solve()) pixels.push_back(p);
			for(Quadrant p : Quadrant(w/2, 0  , w/2, h/2, "1").solve()) pixels.push_back(p);
			for(Quadrant p : Quadrant(0  , h/2, w/2, h/2, "2").solve()) pixels.push_back(p);
			for(Quadrant p : Quadrant(w/2, h/2, w/2, h/2, "3").solve()) pixels.push_back(p);

			console.history().addNew(irr::core::stringw(".*1.*").c_str());
			console.history().addNew(irr::core::stringw(".*(12|21|03|30).*").c_str());
			console.history().addNew(irr::core::stringw("(1[103]|0[12]|2[10]|21)*").c_str());
		}
		~Scene(){}
		void setUp 		(IrrlichtDevice *device);
		void tearDown   (IrrlichtDevice *device);
		void draw 		(IrrlichtDevice *device);
		bool keyDown	(IrrlichtDevice *device, irr::EKEY_CODE keyCode);
		bool keyUp 		(IrrlichtDevice *device, irr::EKEY_CODE keyCode);
		bool OnEvent	(IrrlichtDevice *device, const irr::SEvent& event);

		friend class SceneKeyHandler;
};

//TODO IMPLELEMT TERMINAL MODAL WINDOW
#include <regex>
class SceneKeyHandler{
	public:
		static bool keyUp(irr::IrrlichtDevice *device, Scene& scene, irr::EKEY_CODE keyCode){
			if(keyCode == irr::KEY_OEM_3) { !scene.console.isShown() ? scene.console.show(device) : scene.console.hide(device); return true; }
			if(scene.console.isShown()){
				if(keyCode == irr::KEY_RETURN){
					try{
						std::regex re(irrlight::dev::ConsoleEnterKeyHandler::handle(scene.console, device));
						for(Quadrant& q : scene.pixels) q.rgb(255, 255, 255);
						
						for(Quadrant& q : scene.pixels)
							if(std::regex_match(q.i, re))
								q.rgb(0, 0, 0);

					}catch(const std::regex_error& e){ KLOG(ERROR) << e.what(); }
				}else return irrlight::dev::ConsoleKeyEntryHandler::keyUp(device, scene.console, keyCode);
			}
			return false;
		}
		static bool keyDown(irr::IrrlichtDevice *device, Scene& scene, irr::EKEY_CODE keyCode){
			if(scene.console.isShown()) return irrlight::dev::ConsoleKeyEntryHandler::keyDown(device, scene.console, keyCode);

			return false;
		}
};

#endif /* _FRAGEX_SCENE_HPP_ */
