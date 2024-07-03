def A(n)
  m = Math.sqrt(n).to_i + 1
  h = {}
  (0..m).each{|i|
    (0..m).each{|j|
      k = i * i + 2 * j * j
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

# [nil, [0, 1], [9, 2], [81, 3], [297, 4], [891, 5], [1683, 6], [8019, 7], [5049, 8], [18513, 9], [15147, 10], [88209, 11], [31977, 12], [1185921, 13], [136323, 14], [166617, 15], [95931, 16], [7144929, 17], [351747, 18], nil, [287793, 20], [1499553, 21], [11042163, 22], [2832489, 23], [863379, 24], [20160657, 25], [99379467, 26], [5979699, 27], [2590137, 28], nil, [3165723, 30], nil, [3933171, 32], [121463793, 33], nil, [181445913, 35], [9497169, 36], nil, nil, nil, [11799513, 40], [113614281, 41], [28491507, 42], nil, nil, [53817291, 45], nil, nil, [35398539, 48], nil, nil, nil, nil, nil, [161451873, 54], nil, [106195617, 56], nil, nil, nil, [129794643, 60], nil, nil, nil, [169126353, 64], nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil]
