def A(n)
  m = Math.sqrt(n).floor + 1
  ary = Array.new(n + 1, 0)
  (1..m).each{|i|
    (i + 1..m).each{|j|
      k = i * i + i * j + j * j
      if k <= n
        ary[k] += 1
      end
    }
  }
  ary
end

def B(n)
  ary = A(n * n)
  (0..n).map{|i| ary[i * i]}
end

p B(100)

# n = 11000
# m = 10000
# ary = A(n)
# (0..m).each{|i|
#   print i
#   print ' '
#   puts ary[i]
# }
