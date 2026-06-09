# encoding: utf-8
# Toothpick sequence の格子点に番号を割り当て、
# 原点から反時計回り四角螺旋（右->上->左->下）順に読んだ数列を返す
# a(0) = 4 （原点は特殊値として扱う）

require 'set'

def build_toothpicks(max_steps)
  node_step = {}
  stick_map = {}

  place = ->(x, y, dir, s) {
    k = "#{x},#{y},#{dir}"
    return false if stick_map.key?(k)
    stick_map[k] = s
    pts = dir == 'h' ? [[x-1,y],[x+1,y]] : [[x,y-1],[x,y+1]]
    pts.each { |px,py| node_step["#{px},#{py}"] ||= s + 1 }
    true
  }

  place.(0, 0, 'h', 0)
  active = Set["#{-1},0", "1,0"]

  (1..max_steps).each do |s|
    next_active = Set.new
    placed = []
    active.each do |ek|
      ex, ey = ek.split(',').map(&:to_i)
      on_h = stick_map.key?("#{ex-1},#{ey},h") ||
             stick_map.key?("#{ex+1},#{ey},h") ||
             stick_map.key?("#{ex},#{ey},h")
      nd = on_h ? 'v' : 'h'
      placed << [ex, ey, nd] if place.(ex, ey, nd, s)
    end
    placed.each do |ex, ey, dir|
      ends = dir == 'h' ? [[ex-1,ey],[ex+1,ey]] : [[ex,ey-1],[ex,ey+1]]
      ends.each do |nx, ny|
        hh = stick_map.key?("#{nx},#{ny},h")
        hv = stick_map.key?("#{nx},#{ny},v")
        oh = stick_map.key?("#{nx-1},#{ny},h") || stick_map.key?("#{nx+1},#{ny},h") || hh
        ov = stick_map.key?("#{nx},#{ny-1},v") || stick_map.key?("#{nx},#{ny+1},v") || hv
        next_active.add("#{nx},#{ny}") if (oh && !hv) || (ov && !hh)
      end
    end
    active = next_active
  end

  node_step['0,0'] = 4
  node_step
end

def spiral_ccw(max_r)
  nodes = []
  visited = {}
  push = ->(x, y) {
    return if visited["#{x},#{y}"]
    visited["#{x},#{y}"] = true
    nodes << [x, y]
  }
  push.(0, 0)
  x, y, di, len = 0, 0, 0, 1
  dirs = [[1,0],[0,1],[-1,0],[0,-1]]
  while len <= max_r * 2 + 4
    2.times { dx,dy = dirs[di%4]; len.times { x+=dx; y+=dy; push.(x,y) }; di+=1 }
    len += 1
  end
  nodes
end

def toothpick_spiral_sequence(n, limit: nil)
  limit ||= n * n
  node_step = build_toothpicks(n)
  spiral    = spiral_ccw(n + 3)
  seq = []
  spiral.each do |x, y|
    break if seq.size > limit
    s = node_step["#{x},#{y}"]
    seq << s if s
  end
  seq
end

if __FILE__ == $0
  n = (ARGV[0] || 6).to_i
  seq = toothpick_spiral_sequence(n)
  # puts "n=#{n}, a(0)~a(#{n**2}):"
  # seq.each_with_index { |v, i| print "a(#{i})=#{v}  "; puts if (i+1) % 8 == 0 }
  # puts
  (0..10000).each{|i|
    print i
    print ' '
    puts seq[i] || 'nil'
  }
end
