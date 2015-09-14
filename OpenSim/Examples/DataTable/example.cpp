/* -------------------------------------------------------------------------- *
 *                            OpenSim:  example.cpp                           *
 * -------------------------------------------------------------------------- *
 * The OpenSim API is a toolkit for musculoskeletal modeling and simulation.  *
 * See http://opensim.stanford.edu and the NOTICE file for more information.  *
 * OpenSim is developed at Stanford University and supported by the US        *
 * National Institutes of Health (U54 GM072970, R24 HD065690) and by DARPA    *
 * through the Warrior Web program.                                           *
 *                                                                            *
 * Copyright (c) 2005-2015 Stanford University and the Authors                *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0.         *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */

#include "OpenSim/Common/TimeSeriesTable.h"

int main() {
    using namespace OpenSim;

    TimeSeriesTable table{};

    ValueArray<std::string> value_array{};
    auto& vec = value_array.upd();
    for(unsigned i = 0; i < 5; ++i)
        vec.push_back(SimTK::Value<std::string>{std::to_string(i)});

    TimeSeriesTable::DependentsMetaData dep_metadata{};
    dep_metadata.setValueArrayForKey("labels", value_array);

    table.setDependentsMetaData(dep_metadata);

    SimTK::RowVector_<double> row0{5};
    
    table.appendRow(0.00, row0);

    auto row1 = row0 + 1;

    table.appendRow(0.25, row1);

    auto row2 = row1 + 1;

    table.appendRow(0.50, row2);

    auto row3 = row2 + 1;

    table.appendRow(0.75, row3);
    
    auto row4 = row3 + 1;

    table.appendRow(1.00, row4);

    return 0;
}
