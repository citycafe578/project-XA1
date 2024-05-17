import pygame
import time

def init_joystick():
    pygame.init()
    pygame.joystick.init()
    if pygame.joystick.get_count() == 0:
        print("No joystick found")
        return None
    joystick = pygame.joystick.Joystick(0)
    joystick.init()
    print(f"Joystick initialized: {joystick.get_name()}")
    return joystick

def read_joystick_data(joystick):
    pygame.event.pump()
    axes = [joystick.get_axis(i) for i in range(joystick.get_numaxes())]
    buttons = [joystick.get_button(i) for i in range(joystick.get_numbuttons())]
    hats = [joystick.get_hat(i) for i in range(joystick.get_numhats())]
    return axes, buttons, hats

def main():
    joystick = init_joystick()
    if not joystick:
        return
    
    try:
        while True:
            axes, buttons, hats = read_joystick_data(joystick)
            print(f"Axes: {axes}")
            print(f"Buttons: {buttons}")
            print(f"Hats: {hats}")
            time.sleep(0.1)
    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        pygame.joystick.quit()
        pygame.quit()

if __name__ == "__main__":
    main()
