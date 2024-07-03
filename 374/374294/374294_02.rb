def A(n)
  m = Math.sqrt(n).to_i + 1
  h = {}
  (0..m).each{|i|
    (0..m).each{|j|
      k = i * i + 2 * j * j
      break if k > n
      v = 0
      if i == 0 && j == 0
        v = 0
      elsif i > 0 && j > 0
        v = 2
      else
        v = 1
      end
      if h.key?(k)
        h[k] += v
      else
        h[k] = v
      end
    }
  }
  h.sort
end

n = 1 * 10 ** 8
ary = A(n)

# h[k] = i の 最小のk を出力
p a = (0..100).map{|i| ary.find{|k, j| j == i}}
# (0..100).each{|i|
#   print i
#   print ' '
#   puts a[i] 
# }
