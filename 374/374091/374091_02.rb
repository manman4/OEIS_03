def A(n)
  m = Math.sqrt(n).to_i + 1
  h = {}
  (1..m).each{|i|
    (1..m).each{|j|
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

n = 2 * 10 ** 9
ary = A(n)

# h[k] = i の 最小のk を出力
p a = (0..100).map{|i| ary.find{|k, j| j == i}}
# (0..100).each{|i|
#   print i
#   print ' '
#   puts a[i] 
# }

# [nil, [5, 1], [11, 2], [605, 3], [209, 4], [73205, 5], [2299, 6], [8857805, 7], [6061, 8], [218405, 9], [278179, 10], nil, [66671, 12], nil, [5285401, 14], [26427005, 15], [187891, 16], nil, [1266749, 18], nil, [8067191, 20], nil, nil, nil, [2066801, 24], nil, [36735721, 26], [183678605, 27], [976130111, 28], nil, [153276629, 30], nil, [7703531, 32], nil, nil, nil, [39269219, 36], nil, nil, nil, [250082921, 40], nil, nil, nil, nil, nil, nil, nil, [84738841, 48], nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, [454508329, 64], nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil]
