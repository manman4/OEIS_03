def A(n)
  m = Math.sqrt(n).to_i + 1
  h = {}
  (0..m).each{|i|
    (0..m).each{|j|
      k = i * i + 3 * j * j
      break if k > n
      if h.key?(k)
        h[k] += 1
      else
        h[k] = 1
      end
    }
  }
  h.sort
end

n = 2 * 10 ** 8
ary = A(n)

# h[k] = i の 最小のk を出力
p a = (0..100).map{|i| ary.find{|k, j| j == i}}
# (0..100).each{|i|
#   print i
#   print ' '
#   puts a[i] 
# }
