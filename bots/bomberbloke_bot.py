from bloke import Bot
import random

class PythonBot:
    def __init__(self):
        print("Python bot loaded")

    def handleReset(self):
        print("Reset")

    def update(self, bot: Bot, current_tick: int):
        print(f"Tick {current_tick}")
        commands = ["left", "right", "down", "up", "bomb"]
        chosen_command = random.choice(commands)
        bot.handleCommand(chosen_command)

        nav_grid = bot.getNavGrid()

        print(nav_grid.getNodes())

        actor_location = [round(x) for x in bot.getActorLocation()]

        print(actor_location)

        comp = bot.getConnectedComponentFromNode(nav_grid, actor_location)

        print(nav_grid, comp)
