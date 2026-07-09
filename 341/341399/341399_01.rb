# Number of integer solutions to (x_1)^2 + (x_2)^2 + ... + (x_10)^2 <= n.
def a(n)
  # r2[m] = number of ordered pairs (x, y) of integers with x^2 + y^2 = m.
  r2 = Array.new(n + 1, 0)
  lim = Math.sqrt(n).to_i
  (0..lim).each{|x|
    ylim = Math.sqrt(n - x * x).to_i
    (0..ylim).each{|y|
      m = x * x + y * y
      cx = x == 0 ? 1 : 2
      cy = y == 0 ? 1 : 2
      r2[m] += cx * cy
    }
  }

  conv = ->(f, g){
    h = Array.new(n + 1, 0)
    (0..n).each{|i|
      next if f[i] == 0
      fi = f[i]
      (0..n - i).each{|j|
        gj = g[j]
        h[i + j] += fi * gj if gj != 0
      }
    }
    h
  }

  r4 = conv.call(r2, r2)
  r8 = conv.call(r4, r4)
  r10 = conv.call(r8, r2)

  ary = [1]
  (1..n).each{|i| ary << ary[-1] + r10[i]}
  ary
end

n = 10000
ary = a(n)
(0..n).each{|i|
  print i
  print ' '
  puts ary[i]
}
