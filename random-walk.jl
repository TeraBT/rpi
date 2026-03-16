include("framebuffer-lib.jl")

using Random


function random_walk(steps)
    x = 1000
    y = 500
    traj_x = zeros(Integer, steps)
    traj_y = zeros(Integer, steps)

    for i in 1:steps
        x += rand(Bool) ? 1 : -1
        y += rand(Bool) ? 1 : -1
        traj_x[i] = x
        traj_y[i] = y
    end

    return traj_x, traj_y
end

traj_x, traj_y = random_walk(10000)

fb = open_fb()
white = get_color(100, 100, 100)

for (x, y) in zip(traj_x, traj_y)
    paint_pixel(fb, x, y, white)
end

close_fb(fb)