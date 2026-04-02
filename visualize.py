import json
from pprint import pprint

from matplotlib import pyplot as plt
import matplotlib.animation as animation


MAKE_GIF = False


with open('frames.json', 'r') as file:
    frames = json.load(file)
pprint(frames, sort_dicts=False)

# Find max bacteria speed and energy for color coding
sim_max_speed = 0
sim_max_energy = 0
for frame in frames:
    for b in frame['bacteria']:
        if b['max_speed'] > sim_max_speed:
            sim_max_speed = b['max_speed']
        if b['max_energy'] > sim_max_energy:
            sim_max_energy = b['max_energy']

fig, ax = plt.subplots()
ax.set_aspect('equal', adjustable='box')
ax.set_xlim([0, 100])
ax.set_ylim([0, 100])
def animate(step):
    ax.clear()
    plt.title(f'Step: {step}, '
              f'Bacteria count: {len(frames[step]['bacteria'])}, '
              f'Reproduction evens: {frames[step]['reproduction events']}, '
              f'Death count: {frames[step]['death count']}', loc='left')
    ax.set_xlim([0, 100])
    ax.set_ylim([0, 100])

    circles = []
    for b in frames[step]['bacteria']:
        circle = plt.Circle((b['x'], b['y']), radius=0.7, color=(b['max_speed'] / sim_max_speed,
                                                                     b['max_energy'] / sim_max_energy, 1))
        circles.append(circle)
        ax.add_patch(circle)

    for f in frames[step]['food']:
        circle = plt.Circle((f['x'], f['y']), f['radius'], linewidth=0,
                            color='y', alpha=f['energy'] / (f['radius'] * 20))
        circles.append(circle)
        ax.add_patch(circle)

    return (circles, )

ani = animation.FuncAnimation(fig, animate, repeat=True, frames=len(frames) - 1, interval=50)

if MAKE_GIF:
    writer = animation.PillowWriter(fps=15,
                                    metadata=dict(artist='Me'),
                                    bitrate=1800)
    ani.save('bacteria.gif', writer=writer)

fig_manager = plt.get_current_fig_manager()
fig_manager.window.showMaximized()
plt.show()