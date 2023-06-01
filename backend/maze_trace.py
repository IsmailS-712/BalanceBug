import matplotlib as plt
import math

trace_route = []

def traced_maze(theta, r):

    if 'x' not in trace_route.__dict__:
        trace_route.x = [0]
        trace_route.y = [0]
    else:
        #calc new coordinates based on the angle theta and distance r
        x = trace_route.x[-1] + r * math.cos(theta)
        y = trace_route.y[-1] + r * math.sin(theta)
        trace_route.x.append(x)
        trace_route.y.append(y)

    plt.plot(trace_route.x, trace_route.y)
    plt.xlabel('x')
    plt.ylabel('y')
    plt.title('maze')
    plt.plot(True)
    plt.show()

trace_route(0,10)
trace_route(90,10)
trace_route(45,30)


