#!/usr/bin/ruby

Point = Struct.new(:x, :y)

def sign(n)
    if n > 0
        1
    elsif n < 0
        -1
    else
        0
    end
end

def step(p, v)
    x = p.x + v.x
    y = p.y + v.y
    vx = v.x
    vy = v.y
    if p.x.abs <= p.y.abs
        vy -= sign(y)
    end
    if p.y.abs <= p.x.abs
        vx -= sign(x)
    end
    [Point.new(x, y), Point.new(vx, vy)]
end

FACTOR = 8
p = Point.new(30, 0)
v = Point.new(0, 0)

boost = 0
255.times do |i|
    puts "#{p.x} #{p.y} #{v.x} #{v.y}"
    p, v = step(p, v)

    if i < 4
        v.x += sign(p.x)
        v.y += sign(p.y)
        v.x = 1 if v.x == 0
        v.y = 1 if v.y == 0
        boost += 2
    else
        if p.y > 0 && p.y.abs > p.x.abs && v.x.abs != FACTOR
            v.x -= sign(v.x - -FACTOR)
            boost += 1
        end
        if p.y < 0 && p.y.abs > p.x.abs && v.x.abs != FACTOR
            v.x -= sign(v.x - FACTOR)
            boost += 1
        end
        if p.x > 0 && p.x.abs > p.y.abs && v.y.abs != FACTOR
            v.y -= sign(v.y - -FACTOR)
            boost += 1
        end
        if p.x < 0 && p.x.abs > p.y.abs && v.y.abs != FACTOR
            v.y -= sign(v.y - FACTOR)
            boost += 1
        end
    end
end

puts boost