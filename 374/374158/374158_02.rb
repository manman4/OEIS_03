def A(n)
  m = Math.sqrt(n).to_i + 1
  h = {}
  (1..m).each{|i|
    (1..m).each{|j|
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

n = 12 * 10 ** 8
ary = A(n)

# h[k] = i の 最小のk を出力
p a = (0..100).map{|i| ary.find{|k, j| j == i}}
# (0..100).each{|i|
#   print i
#   print ' '
#   puts a[i] 
# }

# [nil, [4, 1], [91, 2], [28, 3], [196, 4], [31213, 5], [364, 6], [9604, 7], [53599, 8], [2548, 9], [470596, 10], nil, [6916, 12], [33124, 13], [29059303, 14], [124852, 15], [1983163, 16], nil, [48412, 18], nil, [18384457, 20], [6117748, 21], [1623076, 22], nil, [214396, 24], nil, nil, [629356, 27], [900838393, 28], nil, [2372188, 30], [79530724, 31], [85276009, 32], nil, nil, nil, [1500772, 36], [274299844, 37], nil, nil, [11957764, 40], nil, [116237212, 42], nil, nil, [30838444, 45], nil, nil, [7932652, 48], nil, nil, nil, nil, nil, [19510036, 54], nil, nil, nil, nil, nil, [73537828, 60], nil, nil, nil, nil, nil, nil, [585930436, 67], nil, nil, nil, nil, [55528564, 72], nil, nil, nil, nil, nil, nil, nil, nil, [370690684, 81], nil, nil, nil, nil, nil, nil, nil, nil, [955991764, 90], nil, nil, nil, nil, nil, [341104036, 96], nil, nil, nil, nil]
