import json
from pprint import pprint



with open('frames.json', 'r') as file:
    frames = json.load(file)
    pprint(frames, sort_dicts=False)