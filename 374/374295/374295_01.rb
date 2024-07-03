def A(n)
  m = Math.sqrt(n).to_i + 1
  ary = Array.new(n + 1, 0)
  (0..m).each{|i|
    (0..m).each{|j|
      k = i * i + 3 * j * j
      if k <= n
        if i == 0 && j == 0
          ary[k] += 0
        elsif i > 0 && j > 0
          ary[k] += 2
        else
          ary[k] += 1
        end
      end
    }
  }
  ary
end

n = 10 * 10 ** 7
ary = A(n)

p a = (0..100).map{|i| ary.index(i)}
# (0..100).each{|i|
#   print i
#   print ' '
#   puts a[i]
# }
