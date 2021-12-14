import os
import time
import math
import pigpio
import ST7789
from PIL import Image, ImageDraw
from multiprocessing import Process

MAX_STEP = 1000

def load_image(disp, path):
    image = Image.open(path)
    image = image.resize((disp.width, disp.height), resample=Image.LANCZOS)
    return image

def generate_image(disp):
    image = Image.new("RGB", (disp.width, disp.height), (102, 187, 106))
    return image


def fall_ball(disp, background, forward, speed=10):
    image = Image.new("RGB", (disp.width, disp.height), background)
    draw = ImageDraw.Draw(image)
    for i in range(0, 240, speed):
        d = 100
        x = disp.width // 2 - d // 2
        y = i
        draw.rectangle((0, 0, disp.width, disp.height), fill=background, outline=background)
        draw.ellipse((x, y, x+d, y+d), fill=forward, outline=forward)
        disp.display(image)

def rotate_ball(disp, background, forward, speed=10):
    image = Image.new("RGB", (disp.width, disp.height), background)
    draw = ImageDraw.Draw(image)
    d = 100
    r = d // 2
    cx = disp.width // 2 - r
    cy = disp.height // 2 - r
    rotate_r = 50
    for i in range(0, 628, speed):
        x = int(math.cos(i / 100.0) * rotate_r) + cx
        y = int(math.sin(i / 100.0) * rotate_r) + cy
        draw.rectangle((0, 0, disp.width, disp.height), fill=background, outline=background)
        draw.ellipse((x, y, x+d, y+d), fill=forward, outline=forward)
        disp.display(image)

def shutter(disp, background, forward, speed=10):
    image = Image.new("RGB", (disp.width, disp.height), background)
    draw = ImageDraw.Draw(image)
    half = disp.height // 2
    # close
    for i in range(0, half+1, speed):
        draw.rectangle((0, 0, disp.width, disp.height), fill=background, outline=background)  # clear
        draw.rectangle((0, 0, disp.width, i), fill=forward, outline=forward)  # upper
        draw.rectangle((0, disp.height - i, disp.width, disp.height), fill=forward, outline=forward)  # lower
        disp.display(image)
    # open
    for i in range(0, half+1, speed):
        i = half - i
        draw.rectangle((0, 0, disp.width, disp.height), fill=background, outline=background)  # clear
        draw.rectangle((0, 0, disp.width, i), fill=forward, outline=forward)  # upper
        draw.rectangle((0, disp.height - i, disp.width, disp.height), fill=forward, outline=forward)  # lower
        disp.display(image)
    # clear
    draw.rectangle((0, 0, disp.width, disp.height), fill=background, outline=background)  # clear
    disp.display(image)


class Eye:
    def __init__(self, draw, color, cx, cy, eye_r, rotate_r):
        self.draw = draw
        self.color = color
        self.cx = cx
        self.cy = cy
        self.eye_r = eye_r
        self.rotate_r = rotate_r
        self.x, self.y = self._calc_position(0)

    def move(self, step):
        self.x, self.y = self._calc_position(step)

    def paint(self):
        self.draw.ellipse((self.x - self.eye_r, self.y - self.eye_r, self.x + self.eye_r, self.y + self.eye_r),
                          fill=self.color,
                          outline=self.color)

    def _calc_position(self, step):
        rate = step / MAX_STEP
        theta = math.pi * 1.0 * rate
        x = int(math.cos(theta) * self.rotate_r) + self.cx
        y = int(math.sin(theta) * self.rotate_r) + self.cy
        return x, y

class Shutter:
    def __init__(self, draw, color, width, height):
        self.draw = draw
        self.color = color
        self.width = width
        self.height = height
        self.y_low = 0
        self.y_high = height

    def move(self, step):
        rate = step / MAX_STEP
        if rate < 0.5:
            self.y_low = int(self.height * rate)
            self.y_high = self.height - int(self.height * rate)
        elif rate < 1.0:
            self.y_low = self.height - int(self.height * rate)
            self.y_high = int(self.height * rate)
        else:
            self.y_low = 0
            self.y_high = self.height

    def paint(self):
        self.draw.rectangle((0, 0, self.width, self.y_low), fill=self.color, outline=self.color)
        self.draw.rectangle((0, self.height, self.width, self.y_high), fill=self.color, outline=self.color)

        

def eye_blink(disp, back_color, eye_color, shutter_color, speed=10):
    image = Image.new("RGB", (disp.width, disp.height), back_color)
    draw = ImageDraw.Draw(image)
    disp.display(image)  # initialize display
    time.sleep(0.5)

    eye = Eye(draw, eye_color, disp.width / 2, disp.height / 2, 50, 50)
    shutter = Shutter(draw, shutter_color, disp.width, disp.height)
    objects = [eye, shutter]

    # init eye position
    eye.paint()
    disp.display(image)
    time.sleep(0.5)

    # close
    for i in range(0, MAX_STEP * 3, speed):
        # update positions
        if i < MAX_STEP:
            if (i / speed) % 2 == 1:  # speed up only for shutter
                continue
            shutter.move(i)
        elif i < MAX_STEP * 2:
            eye.move(i - MAX_STEP)
        else:
            if (i / speed) % 2 == 1:  # speed up only for shutter
                continue
            shutter.move(i - MAX_STEP * 2)

        # update image
        draw.rectangle((0, 0, disp.width, disp.height), fill=back_color, outline=back_color)  # clear
        for o in objects:
            o.paint()
        disp.display(image)

    # wait input
    print("waiting input...")
    input()

    # clear monitor
    draw.rectangle((0, 0, disp.width, disp.height), fill=back_color, outline=back_color)
    disp.display(image)


def eye_gif(disp, image_path, speed=10):
    image = Image.open(image_path)

    count = 0
    frame = 0
    time_start = time.time()
    while True:
        try:
            image.seek(frame)
            disp.display(image)
            frame += 1
            # time.sleep(0.05)
        except EOFError:
            frame = 0

        count += 1
        duration = time.time() - time_start
        if count % 120 == 0:
            print("Time: {:8.3f}, FPS: {:8.3f}".format(duration, count / duration))

def eye_gif(disp, image_path, speed=10):
    image = Image.open(image_path)

    count = 0
    frame = 0
    time_start = time.time()
    while True:
        try:
            image.seek(frame)
            disp.display(image)
            frame += 1
            # time.sleep(0.05)
        except EOFError:
            frame = 0

        count += 1
        duration = time.time() - time_start
        if count % 120 == 0:
            print("Time: {:8.3f}, FPS: {:8.3f}".format(duration, count / duration))

def eye_images(disp, movie_id):

    count = 0
    frame = 0
    NUM_FRAME = 176
    time_start = time.time()

    dir_path = "eye_douga/eye{}".format(movie_id)
    prefix = ["eye_", "eye2_"]
    file_names = [prefix[movie_id - 1] + str(i).zfill(5) + ".png" for i in range(NUM_FRAME)]
    images = [Image.open(dir_path + os.sep + f) for f in file_names]

    print("Start eye{}".format(movie_id))
    while True:
        disp.display(images[frame])
        frame += 1

        if frame == NUM_FRAME:
            frame = 0

        count += 1
        duration = time.time() - time_start
        if count % 120 == 0:
            print("Time: {:8.3f}, FPS: {:8.3f}".format(duration, count / duration))


if __name__ == "__main__":
    disp = ST7789.ST7789(
            port=0,
            cs=0,  # no use
            rst=5, # RES <-> GPIO 5
            dc=6,  # DC  <-> GPIO 6
            backlight=13,  # BLK <-> GPIO 13
            spi_speed_hz=80*1000*1000)

    # change SPI mode
    disp._spi.mode = 3
    disp.reset()
    disp._init()

    # generate image and show
    # eye_blink(disp, back_color=(0, 0, 0), eye_color=(100, 200, 100), shutter_color=(80, 80, 80), speed=100) 
    # eye_gif(disp, "eye_douga/eye1.gif")
    movie_id = 1
    while True:
        p = Process(target=eye_images, args=(disp, movie_id))
        p.start()

        key = input()
        try:
            movie_id = int(key)
            p.terminate()
            p.join()
        except Error as e:
            print(e)

