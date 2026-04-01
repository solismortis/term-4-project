import json
from pprint import pprint

from matplotlib import pyplot as plt
import matplotlib.animation as animation


with open('frames.json', 'r') as file:
    frames = json.load(file)
pprint(frames, sort_dicts=False)

fig, ax = plt.subplots()
ax.set_xlim([0, 100])
ax.set_ylim([0, 100])

def animate(frame):
    bacteria_xs = []
    bacteria_ys = []
    for b in frames[frame]['bacteria']:
        bacteria_xs.append(b['x'])
        bacteria_ys.append(b['y'])
    return (plt.scatter(bacteria_xs, bacteria_ys),)

ani = animation.FuncAnimation(fig, animate, repeat=True, frames=len(frames) - 1, interval=50)
plt.show()