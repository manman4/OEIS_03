def A(n)
  m = Math.sqrt(n).floor + 1
  ary = Array.new(n + 1, 0)
  (0..m).each{|i|
    (i..m).each{|j|
      k = i * i + 3 * i * j + j * j
      if k <= n
        ary[k] += 1
        p [k, i, j]
      end
    }
  }
  ary
end

p A(130)

# n = 11000
# m = 10000
# ary = A(n)
# (0..m).each{|i|
#   print i
#   print ' '
#   puts ary[i]
# }
