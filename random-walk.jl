using Random

function random_walk(steps)
    x = 0
    trajectory = zeros(steps)

    for i in 1:steps
        x += rand(Bool) ? 1 : -1
        trajectory[i] = x
    end

    return trajectory
end

trajectory = random_walk(10000)
println(trajectory[1:10])

# TODO: Print to framebuffer via C functions.