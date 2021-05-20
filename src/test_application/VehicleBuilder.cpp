/**
 * Open Space Program
 * Copyright © 2019-2020 Open Space Program Project
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "VehicleBuilder.h"

using testapp::VehicleBuilder;
using testapp::partindex_t;
using osp::BlueprintPart;
using osp::BlueprintMachine;
using osp::PrototypePart;
using osp::PCompMachine;
using osp::DependRes;
using osp::Vector3;
using osp::Quaternion;
using osp::machine_id_t;

partindex_t VehicleBuilder::add_part(
        DependRes<PrototypePart>& prototype,
        const Vector3& translation,
        const Quaternion& rotation,
        const Vector3& scale)
{
    uint32_t protoIndex = m_vehicle.m_prototypes.size();

    // check if the Part Prototype is added already.
    for (size_t i = 0; i < protoIndex; i ++)
    {
        const DependRes<PrototypePart>& dep = m_vehicle.m_prototypes[i];

        if (dep == prototype)
        {
            // prototype was already added to the list
            protoIndex = i;
            break;
        }
    }

    if (protoIndex == m_vehicle.m_prototypes.size())
    {
        // Add the unlisted prototype to the end
        m_vehicle.m_prototypes.emplace_back(prototype);
    }

    size_t numMachines = prototype->m_protoMachines.size();

    // We now know which part prototype to initialize, create it
    uint32_t blueprintIndex = m_vehicle.m_blueprints.size();
    BlueprintPart &rPart = m_vehicle.m_blueprints.emplace_back(
                protoIndex, numMachines, translation, rotation, scale);

    // Add default machines from part prototypes

    for (size_t i = 0; i < numMachines; i ++)
    {
        PCompMachine const &protoMach = prototype->m_protoMachines[i];
        machine_id_t id = protoMach.m_type;
        m_vehicle.m_machines.resize(std::max(m_vehicle.m_machines.size(),
                                             size_t(id + 1)));

        BlueprintMachine &rBlueprintMach = m_vehicle.m_machines[id].emplace_back();
        rBlueprintMach.m_protoMachineIndex = i;
        rBlueprintMach.m_blueprintIndex = blueprintIndex;
        rBlueprintMach.m_config = protoMach.m_config;
    }

    return blueprintIndex;
}

void VehicleBuilder::add_wire(
        uint32_t fromPart, uint32_t fromMachine, osp::WireOutPort fromPort,
        uint32_t toPart, uint32_t toMachine, osp::WireInPort toPort)
{
    m_vehicle.m_wires.emplace_back(fromPart, fromMachine, fromPort,
                         toPart, toMachine, toPort);
}