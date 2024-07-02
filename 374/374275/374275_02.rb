def A(n)
  m = Math.sqrt(n).to_i + 1
  h = {}
  (0..m).each{|i|
    (i..m).each{|j|
      k = i * i + 3 * i * j + j * j
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

n = 10 * 10 ** 8
ary = A(n)

# h[k] = i の 最小のk を出力
p a = (0..100).map{|i| ary.find{|k, j| j == i}}
# (0..100).each{|i|
#   print i
#   print ' '
#   puts a[i] 
# }

# [nil, [0, 1], [121, 2], [2299, 3], [6061, 4], [43681, 5], [66671, 6], [33659659, 7], [187891, 8], [1266749, 9], [8067191, 10], [639533521, 11], [2066801, 12], nil, [36735721, 14], [153276629, 15], [7703531, 16], nil, [39269219, 18], nil, [250082921, 20], nil, nil, nil, [84738841, 24], nil, nil, nil, nil, nil, nil, nil, [454508329, 32], nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil]
