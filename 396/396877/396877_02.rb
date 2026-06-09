# encoding: utf-8
#
# a(n): 反時計回り四角螺旋のn番目の格子点が
#        初めて棒の端点になるステップ番号
#
# 閉じた式: シミュレーション不要、O(sqrt(n))で計算可能
#
# m = ceil(|x|/2), k = ceil(|y|/2) として
#   x奇, y偶  ->  4 * max(m-1, k) + 1
#   x偶, y奇  ->  4 * max(m,   k) - 1
#   x奇, y奇  ->  4 * max(m,   k) - 2
#   x偶, y偶  ->  4 * max(m,   k)     ※ (0,0) のみ例外で 4
#
# 性質: a(k^2) = k  (k >= 1)

def a_xy(x, y)
  ax, ay = x.abs, y.abs
  m = (ax + 1) / 2   # ceil(|x|/2)
  k = (ay + 1) / 2   # ceil(|y|/2)
  if ax.odd? && ay.even?
    4 * [m - 1, k].max + 1
  elsif ax.even? && ay.odd?
    4 * [m, k].max - 1
  elsif ax.odd? && ay.odd?
    4 * [m, k].max - 2
  else
    [m, k].max == 0 ? 4 : 4 * [m, k].max
  end
end

# 反時計回り四角螺旋で n 番目の座標を返す
def spiral_xy(n)
  return [0, 0] if n == 0
  x, y, di, len, count = 0, 0, 0, 1, 0
  dirs = [[1,0],[0,1],[-1,0],[0,-1]]
  loop do
    2.times do
      dx, dy = dirs[di % 4]
      len.times do
        x += dx; y += dy
        count += 1
        return [x, y] if count == n
      end
      di += 1
    end
    len += 1
  end
end

# a(n): 螺旋n番目の値
def a(n)
  a_xy(*spiral_xy(n))
end

# 数列を limit 項生成
def sequence(limit)
  (0..limit).map { |n| a(n) }
end

if __FILE__ == $0
  n = (ARGV[0] || 10).to_i

  # puts "a(k^2) = k の確認:"
  # (1..10).each { |k| print "a(#{k**2})=#{a(k**2)}  " }
  # puts

  # puts "\na(0)~a(#{n**2}):"
  seq = sequence(n * n)
  (0..10000).each{|i|
    print i
    print ' '
    puts seq[i] || 'nil'
  }
end
