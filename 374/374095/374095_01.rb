def A(n)
  m = Math.sqrt(n).to_i + 1
  ary = Array.new(n + 1, 0)
  (1..m).each{|i|
    (i + 1..m).each{|j|
      k = i * i + 3 * i * j + j * j
      if k <= n
        ary[k] += 1
      end
    }
  }
  ary
end

n = 20 * 10 ** 7
ary = A(n)

p a = (0..100).map{|i| ary.index(i)}
# (0..100).each{|i|
#   print i
#   print ' '
#   puts a[i]
# }
