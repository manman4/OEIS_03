def A(n)
  m = Math.sqrt(2 * n).floor + 1
  ary = Array.new(n + 1, 0)
  (1..m).each{|i|
    (1..m).each{|j|
      if i.gcd(j) == 1
        k = i * i + 3 * i * j + j * j
        if k <= n
          ary[k] += 1
        end
      end
    }
  }
  ary
end

ary = A(500003)
p a = (0..100).map{|i| ary.index(i)}
# n = 11000
# m = 10000
# ary = A(n)
# (0..m).each{|i|
#   print i
#   print ' '
#   puts ary[i]
# }
