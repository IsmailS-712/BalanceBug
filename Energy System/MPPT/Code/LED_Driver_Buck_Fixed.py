from machine import Pin, ADC, PWM

vret_pin = ADC(Pin(26))
vout_pin = ADC(Pin(28))
vin_pin = ADC(Pin(27))
pwm = PWM(Pin(0))
pwm.freq(100000)
pwm_en = Pin(1, Pin.OUT)


count = 0
pid_input = 0
pwm_out = 0
pwm_ref = 0
setpoint = 0.0
delta = 0.05

current_limit = 0.015 #whatever this is


def saturate(duty):
    if duty > 62500:
        duty = 62500
    if duty < 100:
        duty = 100
    return duty

while True:
    
    pwm_en.value(1)

    vin = vin_pin.read_u16() * 3.3 / 65536
    vout = vout_pin.read_u16() * 3.3 / 65536
    vret = vret_pin.read_u16() * 3.3 / 65536
    count = count + 1
    

    iout=vret/1.02

    if iout > 2 * current_limit:
        pid_input /= 2
    
    if iout > current_limit:
       pid_input -= 2000
       pid_input = int(pid_input)
    elif iout <= current_limit:
       pid_input += 1000



    pwm_out = saturate(pid_input)
    pwm.duty_u16(pwm_out)
    
    Vin = 5 * vin * 3.3 / 65535
    Vout = 5 * vout * 3.3 / 65535
    Vret = 5 * vret * 3.3 / 65535
    Pwm_out = pwm_out * 100 / 65535
    
    
    
    if count > 2000:
        print("Vin = {:.2f}".format(Vin))
        print("Vout = {:.2f}".format(Vout))
        print("Vret = {:.2f}".format(Vret))
        print("Duty = {:.2f}".format(Pwm_out))
        print('iout = {:.2f}'.format(iout))
        count = 0