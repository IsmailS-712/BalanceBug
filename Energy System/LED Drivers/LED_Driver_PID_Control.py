from machine import Pin, ADC, PWM

 

vret_pin = ADC(Pin(26))

vout_pin = ADC(Pin(28))

vin_pin = ADC(Pin(27))

test_pin = Pin(3, mode=Pin.OUT)

pwm = PWM(Pin(0))

pwm.freq(100000)

pwm_en = Pin(1, Pin.OUT)

 

voltage_ratio = 249/1249  # 0.19935949

 

count = 0

pwm_out = 0

pwm_ref = 0

 

led = Pin("LED", Pin.OUT)

led.on()

 

CURRENT_LIMIT = 0.29

 

# CURRENT PID CONTROL VARS

KPI = 0.0

KDI = 0.0

KII = 0.0

 

TS  = 0.001

 

# CURRENT VARS

CURRENT_ERROR = 0.0

CURRENT_DERIVATIVE_ERROR = 0.0

CURRENT_INTEGRAL_ERROR = 0.0

CURRENT_LAST_ERROR = 0.0

CURRENT_PID_OUTPUT = 0.0

PREVIOUS_CURRENT_PID_OUTPUT = 0.0

 

LAST_OUTPUT_CURRENT = 0.0

 

TARGET_CURRENT = 0.03

 

def saturate(duty):

    if duty > 62500:

        duty = 62500

    if duty < 100:

        duty = 100

    return duty

 

interrupt_flag = False

 

def interrupt_callback(timer):

    global interrupt_flag

    interrupt_flag = True

 

#timer = machine.Timer(0)

#timer.init(period=800, mode=Timer.PERIODIC, callback=interrupt_callback)

 

while True:

 

    KPI = 45.0
    
    KDI = 0.0
    
    KII = 0.00054
 

    pwm_en.value(1)

   

    if test_pin.value() == 0:

        test_pin.on()

    else:

        test_pin.off()

 

    #if interrupt_flag:

 

    interrupt_flag = False

 

    vin   = 5*(vin_pin.read_u16()  * (3.3/65535))   # input voltage

    vout  = 5*(vout_pin.read_u16() * (3.3/65535))   # output voltage

    vret  = 0.7*(vret_pin.read_u16() * (3.3/65535))   # current reference pin (1.02ohm)

    current  = vret / 1.02

    count = count + 1

 

    # PI(D) CONTROL

    # target current = 0.15A

    CURRENT_ERROR = TARGET_CURRENT - current

    CURRENT_INTEGRAL_ERROR += CURRENT_ERROR * TS

    CURRENT_DERIVATIVE_ERROR = (CURRENT_ERROR - CURRENT_LAST_ERROR) / TS

    CURRENT_LAST_ERROR = CURRENT_ERROR # set the last error to the current error (as it will now be the last)

   

 

    # antiwindup

    if (LAST_OUTPUT_CURRENT >= CURRENT_LIMIT):

        KII = 0.0

    elif (LAST_OUTPUT_CURRENT <= 0):
        
        KII = 0.0

 

    # this output is used to control the duty cycle (adds to the last control signal)

    CURRENT_PID_OUTPUT += (KPI*CURRENT_ERROR) + (KDI*CURRENT_DERIVATIVE_ERROR) + (KII*CURRENT_INTEGRAL_ERROR)

    LAST_OUTPUT_CURRENT = current

   

    pwm_ref = int((CURRENT_PID_OUTPUT))

    pwm_out = saturate(pwm_ref)

    pwm.duty_u16(pwm_out)

   

    #print("current", current)

   

    if count > 2000:

        print("Vin  = {:.5f}".format(vin))

        print("Vout = {:.5f}".format(vout))

        print("Vret = {:.5f}".format(vret))

        print("Duty = {:.5f}".format(pwm_out))

        print("last output current = {:.5f}".format(LAST_OUTPUT_CURRENT))

#          print("Current = {:.5f}".format(current))

        #print("PID OUTPUT = {:.5f}".format(CURRENT_PID_OUTPUT))

#          # print("INTEGRAL ERROR = {:.5f}".format(CURRENT_INTEGRAL_ERROR))

#          # print("DY/DX ERROR = {:.5f}".format(CURRENT_DERIVATIVE_ERROR))

#          print("")

        count = 0
