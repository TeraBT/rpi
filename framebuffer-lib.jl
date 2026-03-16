
const fblib = "./fblib.so"

struct Framebuffer
    fb_fd::Cint
    fb_ptr::Ptr{UInt16}
    height::Csize_t
    width::Csize_t
    line_length::Csize_t
    bits_per_pixel::Csize_t
end

function open_fb()
    fb = Ref{Framebuffer}()

    ccall((:open_fb, fblib),
        Cvoid,
        (Ref{Framebuffer},),
        fb)

    return fb[]
end

function close_fb(fb::Framebuffer)
    ccall((:close_fb, fblib),
        Cvoid,
        (Ref{Framebuffer},),
        Ref(fb))
end

function get_color(r::Integer, g::Integer, b::Integer)
    ccall((:get_color, fblib),
        UInt16,
        (Csize_t, Csize_t, Csize_t),
        r, g, b)
end

function paint_pixel(fb::Framebuffer, x::Integer, y::Integer, color::UInt16)
    ccall((:paint_pixel, fblib),
        Cvoid,
        (Ref{Framebuffer}, Cint, Cint, UInt16),
        Ref(fb), x, y, color)
end
