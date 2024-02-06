/* Horus Vision Rendering Engine
 * Copyright (C) 2024 Theo Baudoin Malnoe
 *
 * This file is part of the Horus Vision Rendering Engine.
 *
 * All files in the Horus Vision Rendering Engine project are
 * distributed under the terms of the GNU General Public License v3.0.
 *
 * Horus Vision Rendering Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Horus Vision Rendering Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Horus Vision Rendering Engine. If not, see <https://www.gnu.org/licenses/>.
 *
 * Portions of this file may include code from official sources or other third-party contributions,
 * which are also distributed under the terms of the GNU General Public License.
 *
 * DISCLAIMER: The authors and maintainers of Horus Vision Rendering Engine make no representations
 * or warranties about the suitability of the software, either express or implied, including but not
 * limited to the implied warranties of merchantability, fitness for a particular purpose, or
 * non-infringement.
 *
 * CONTRIBUTING: For guidelines on contributing to Horus Vision Rendering Engine, please refer to
 * https://github.com/TheoBaudoinLighting/Horus-Vision/issues.
 *
 * DOCUMENTATION: For detailed documentation and usage instructions, visit
 * https://github.com/TheoBaudoinLighting/Horus-Vision/blob/master/README.md.
 *
 */

// Projects Include
#include "hrs_engine.h" 
#include "hrs_timer.h"

int main()
{
	HorusEngine& Engine = HorusEngine::GetInstance();
	HorusTimerManager::GetInstance().StartTimer("EngineInit");

	Engine.Init(1920, 1080, "Horus Vision", "DefaultUserSave.json");

	while (Engine.IsRunning())
	{
		Engine.PreRender();
		Engine.Render();
		Engine.PostRender();
	}

	spdlog::info("Exiting...");
	return 0;
}