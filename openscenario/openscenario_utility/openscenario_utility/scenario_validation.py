#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright 2021 Tier IV, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from pathlib import Path
from pkg_resources import resource_string
from sys import exit
from openscenario_utility.conversion import convert

import argparse
import xmlschema


def find_in_dict(key, dictionary):

    if not isinstance(dictionary, dict):
        return []

    values = list()
    if key in dictionary:
        values.append(dictionary[key])
    for k, v in dictionary.items():
        if k == key:
            values.append(v)
        if isinstance(v, list):
            for i in v:
                for j in find_in_dict(key, i):
                    values.append(j)
        elif isinstance(v, dict):
            for result in find_in_dict(key, v):
                values.append(result)
    return values


class ScenarioValidator:
    def __init__(self):
        self.warning_message = ''
        self.schema = xmlschema.XMLSchema(
            resource_string(
                __name__, "resources/OpenSCENARIO.xsd").decode("utf-8")
        )

    def __call__(self, path: Path):
        return None

    def get_warning_message(self):
        return self.warning_message


class ReachPositionConditionValidator(ScenarioValidator):
    def __init__(self):
        super().__init__()
        self.warning_message = "ReachPositionCondition position does not match the last position of the path"

    def __call__(self, scenario):

        # Search for the last Routing Action destionation:
        last_destination = find_in_dict(
            'Position', find_in_dict('RoutingAction', scenario)[-1])

        # Search for ReachPositionCondition:
        reach_position_conditon = find_in_dict(
            'Position', find_in_dict('ReachPositionCondition', scenario)[-1])

        return reach_position_conditon == last_destination


def validate_file():

    schema = xmlschema.XMLSchema(
        resource_string(
            __name__, "resources/OpenSCENARIO.xsd").decode("utf-8")
    )

    scenario_validators = list()
    scenario_validators.append(ReachPositionConditionValidator())

    parser = argparse.ArgumentParser(
        description="Validate if the Scenario Files pass the conditions checked by validators"
    )
    parser.add_argument("file_paths", type=Path, nargs="+")

    args = parser.parse_args()

    for path in args.file_paths:
        scenarios = []
        paths = []
        if path.suffix == '.xosc':
            paths.append(path)
            scenarios.append(schema.to_dict(str(path)))
        else:
            output = Path("/tmp").joinpath("xosc")
            for converted_path in convert(path, output, verbose=False):
                paths.append(converted_path)
                scenarios.append(schema.to_dict(str(converted_path)))

        for scenario, path in zip(scenarios, paths):
            for validator in scenario_validators:
                result = validator(scenario)
                print(str(path) + " : " + str(result))
