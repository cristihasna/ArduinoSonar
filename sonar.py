import tkinter as tk
import serial
import serial.tools.list_ports
from math import sin, cos, acos, pi, sqrt

canvas_w = 600 
canvas_h = 600
canvas_bg = '#081A0B'
canvas_circle_bd = '#91E1BB'
canvas_line_fill = '#2A883D'
canvas_grid_bg = '#0B240F'
canvas_pointer = '#fff'
canvas_target = '#124C1E'
button_bg_active = '#ccc'
button_fg_active = '#000'
button_bg = '#fff'
button_fg = '#000'

connection = None
master = None
canvas = None
rot_button = None
buzzer_button = None
current_angle = None
current_target = None
current_distance = None

class Canvas(tk.Canvas):
    def __init__(self, master, click_ev_callback):
        canvas_options = {
            'width': canvas_w,
            'height': canvas_h,
            'bg': canvas_bg
        }

        self.click_ev_callback = click_ev_callback
        tk.Canvas.__init__(self, master, canvas_options)
        self.bind("<Button 1>", self.handle_click)
        self.pointer = None
        self.target = None
        self.draw_background()

    def handle_click(self, event):
        angle = self.deg_from_point(event.x, event.y)
        self.after(0, lambda: self.draw_target(angle))
        self.click_ev_callback(angle)

    def draw_grid(self, lines):
        for i in range(0, canvas_w, canvas_w // lines):
            self.create_line(i, 0, i, canvas_h, fill=canvas_grid_bg)
        for i in range(0, canvas_h, canvas_h // lines):
            self.create_line(0, i, canvas_w, i, fill=canvas_grid_bg)

    def draw_background(self):
        self.draw_grid(20)
        # 0deg line
        self.create_line(canvas_w / 2, 0, canvas_w / 2, canvas_h / 2 - 30, fill=canvas_line_fill)
        self.create_text(canvas_w / 2 + 2, canvas_h / 16 + 5, text='0deg', fill=canvas_line_fill, anchor=tk.NW)
        #90deg line
        self.create_line(canvas_w / 2 + 30, canvas_h / 2, canvas_w, canvas_h / 2, fill=canvas_line_fill)
        self.create_text(canvas_w - canvas_w / 16 + 2, canvas_h / 2 + 8, text='90deg', fill=canvas_line_fill)
        #180deg line
        self.create_line(canvas_w / 2, canvas_h / 2 + 30, canvas_w / 2, canvas_h, fill=canvas_line_fill)
        self.create_text(canvas_w / 2 + 2, canvas_h - canvas_h / 16 + 5, text='180deg', fill=canvas_line_fill, anchor=tk.NW)
        #270deg line
        self.create_line(canvas_w / 2 - 30, canvas_h / 2, 0, canvas_h / 2, fill=canvas_line_fill)
        self.create_text(canvas_w / 16 + 2, canvas_h / 2 + 8, text='270deg', fill=canvas_line_fill)
        # draw circles | 200cm delimiter
        self.create_oval(2, 2, canvas_w, canvas_h, outline=canvas_circle_bd)
        self.create_text(canvas_w / 2, 10, text='200cm', fill=canvas_circle_bd)
        # 100cm delimiter
        self.create_oval(canvas_w / 4 + 2, canvas_h / 4 + 2, canvas_w - canvas_w / 4, canvas_h - canvas_h / 4, outline=canvas_circle_bd)
        self.create_text(canvas_w / 2, canvas_h / 4 + 10, text='100cm', fill=canvas_circle_bd)
        # 150cm delimiter
        self.create_oval(canvas_w / 8 + 2, canvas_h / 8 + 2, canvas_w - canvas_w / 8, canvas_h - canvas_h / 8, outline=canvas_circle_bd)
        self.create_text(canvas_w / 2, canvas_h / 8 + 10, text='150cm', fill=canvas_circle_bd)
        # 50cm delimiter
        self.create_oval(3 * canvas_w / 8 + 2, 3 * canvas_h / 8 + 2, canvas_w - 3 * canvas_w / 8, canvas_h - 3 * canvas_h / 8, outline=canvas_circle_bd)
        self.create_text(canvas_w / 2, 3 * canvas_h / 8 + 10, text='50cm', fill=canvas_circle_bd)
        # center point
        self.create_oval(canvas_w / 2 - 5, canvas_h / 2 - 5, canvas_w / 2 + 5, canvas_h / 2 + 5, outline=canvas_circle_bd, fill=canvas_circle_bd)
        self.create_text(canvas_w / 2, canvas_h / 2 + 13, text='0', fill=canvas_circle_bd)

    def distance_bt_points(self, x1, y1, x2, y2):
        return sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2)

    def deg_from_point(self, x, y):
        A = (canvas_w / 2, canvas_h / 2)
        B = (canvas_w / 2, 0)
        C = (x, y)
        a = self.distance_bt_points(B[0], B[1], C[0], C[1])
        b = self.distance_bt_points(A[0], A[1], C[0], C[1])
        c = self.distance_bt_points(A[0], A[1], B[0], B[1])
        angle_rad = acos((b * b + c * c - a * a) / (2 * b * c))
        angle = angle_rad * 180 / pi
        if C[0] < canvas_w / 2:
            angle = 360 - angle
        return angle

    def point_on_circle(self, angle, radius):
        radians = (angle - 90) * pi / 180
        return (canvas_w / 2 + radius * cos(radians), canvas_h / 2 + radius * sin(radians))

    def draw_pointer(self, deg):
        line_x, line_y = self.point_on_circle(deg, canvas_w / 2)
        if not self.pointer:
            self.pointer = self.create_line(canvas_w / 2, canvas_h / 2, line_x, line_y, fill=canvas_pointer, width=5)
        else:
            self.coords(self.pointer, canvas_w / 2, canvas_h / 2, line_x, line_y)
    
    def draw_target(self, deg):
        line_x, line_y = self.point_on_circle(deg, canvas_w / 2)
        if not self.target:
            self.target = self.create_line(canvas_w / 2, canvas_h / 2, line_x, line_y, fill=canvas_target, width=3, tag='target')
        else:
            self.coords(self.target, canvas_w / 2, canvas_h / 2, line_x, line_y)

    def remove_target(self):
        self.delete('target')
        self.target = None

class Button(tk.Button):
    def __init__(self, master, text, state, callback):
        self.callback = callback
        self.name = text
        tk.Button.__init__(self, master, text=text, bd=0, padx=10, pady=10, width=30, anchor=tk.W)
        self.master = master
        self.bind("<Button 1>", self.handle_click)
        self.state = state
        self.set_state(state)
    
    def set_state(self, state):
        self.state = state
        if state == 1 or state == 2:
            self['text'] = self.name + ': Pause'
            self['bg'] = button_bg_active
            self['fg'] = button_fg_active
        else:
            self['text'] = self.name + ': Continue'
            self['bg'] = button_bg
            self['fg'] = button_fg
    
    def handle_click(self, event):
        self.callback()

    def get_state(self):
        return self.state

class Label(tk.Label):
    def __init__(self, master, text, unit):
        tk.Label.__init__(self, master, text=text, width=30, anchor=tk.W)
        self.unit = unit
        self.name = text
    
    def set_value(self, value):
        self['text'] = '{}: {} {}'.format(self.name, str(value), self.unit)

def find_arduino_port():
    ports = list(serial.tools.list_ports.comports())
    for port in ports:
        if 'Arduino' in port.description:
            return port.device
    return None

def clicked_canvas(deg):
    string = 'R|{}'.format(int(deg)).encode()
    print("new target: {}".format(string))
    connection.write(string)

def handle_rotation():
    if rot_button.get_state() == 0 or rot_button.get_state() == 2:
        # continue if paused or not rotating
        connection.write(b'C')
        print("wrinting C")
    else:
        connection.write(b'P')
        print("writing P")
    
def handle_buzzer():
    if buzzer_button.get_state() == 1 or buzzer_button.get_state() == 2:
        # disable buzzer if enabled
        buzzer_button['text'] = 'Enable Buzzer'
        buzzer_button.set_state(0)
        connection.write(b'S')
    else:
        buzzer_button['text'] = 'Disable Buzzer'
        if buzzer_button.get_state() == 0:
            buzzer_button.set_state(1)
            connection.write(b'B')

def loop():
    if connection.in_waiting:
        line:str = connection.readline().decode()
        values = line.rstrip().split('|')
        try:
            print(values)
            if len(values) == 4:
                angle = float(values[0])
                distance = float(values[1])
                target = float(values[2])
                rotating = int(values[3])
                canvas.draw_pointer(angle)
                canvas.draw_target(target)
                rot_button.set_state(rotating)
                current_angle.set_value(angle)
                current_target.set_value(target)
                current_distance.set_value(distance)

        except ValueError:
            pass
    master.after(1, loop)

if __name__ == "__main__":

    port = find_arduino_port()
    if not port:
        print("No arduino connected!")
        exit(1)

    try:
        connection = serial.Serial(port, 9600, timeout=1)
    except serial.serialutil.SerialException:
        print("Could not open port!")
        exit(1)

    master = tk.Tk()
    left_frame = tk.Frame(master)
    left_frame.pack(side=tk.LEFT)

    canvas = Canvas(left_frame, clicked_canvas)
    canvas.pack()

    right_frame = tk.Frame(master, padx=20, pady=20)
    right_frame.pack(side=tk.RIGHT, anchor=tk.NW)

    rot_button = Button(right_frame, text='Rotation', state=1, callback=handle_rotation)
    rot_button.grid(row=0, column=0, padx=10, pady=10)

    buzzer_button = Button(right_frame, text='Buzzer', state=1, callback=handle_buzzer)
    buzzer_button.grid(row=1, column=0, padx=10, pady=10)

    current_angle = Label(right_frame, text='Angle', unit='deg')
    current_angle.grid(row=2, column=0, padx=10, pady=10)

    current_target = Label(right_frame, text='Target', unit='deg')
    current_target.grid(row=3, column=0, padx=10, pady=10)

    current_distance = Label(right_frame, text='Distance', unit='cm')
    current_distance.grid(row=4, column=0, padx=10, pady=10)
    
    master.after(1, loop)
    tk.mainloop()
    loop()