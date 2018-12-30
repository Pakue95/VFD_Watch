from machine import *
import mcp

pin_24V = Pin(2, Pin.OUT)
pin_HEAT_nSleep = Pin(18, Pin.OUT)
pin_HEAT_INT1 = Pin(5, Pin.OUT)
pin_HEAT_INT2 = Pin(17, Pin.OUT)

io = mcp.MCP23017(gpioScl=22, gpioSda=21)

for pin in range(0,16):
    io.setup(pin, mcp.OUT)

pin_24V.value(1)
pin_HEAT_nSleep.value(1)
pin_HEAT_INT1.value(1)
HEAT2_PWM = PWM(pin_HEAT_INT2)
HEAT2_PWM.freq(1000)
HEAT2_PWM.duty(200)
