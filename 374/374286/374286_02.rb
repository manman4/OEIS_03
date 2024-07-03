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

# [nil, [0, 1], [4, 2], [28, 3], [1729, 4], [196, 5], [364, 6], [1529437, 7], [9604, 8], [2548, 9], [593047, 10], [470596, 11], [6916, 12], [68574961, 13], [33124, 14], [124852, 15], [1983163, 16], nil, [48412, 18], nil, [18384457, 20], [6117748, 21], nil, [1623076, 23], [214396, 24], nil, nil, [629356, 27], nil, nil, [2372188, 30], nil, [79530724, 32], nil, nil, nil, [1500772, 36], nil, nil, nil, nil, [11957764, 41], [116237212, 42], nil, nil, [30838444, 45], nil, nil, [7932652, 48], nil, nil, nil, nil, nil, [19510036, 54], nil, nil, nil, nil, nil, [73537828, 60], nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, [55528564, 72], nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil]
