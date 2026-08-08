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
