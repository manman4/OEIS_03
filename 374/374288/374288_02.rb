def A(n)
  m = Math.sqrt(n).to_i + 1
  h = {}
  (0..m).each{|i|
    (0..m).each{|j|
      k = i * i + 5 * j * j
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

# [nil, [0, 1], [9, 2], [81, 3], [189, 4], [441, 5], [1449, 6], [10206, 7], [3969, 8], [12789, 9], [13041, 10], [35721, 11], [30429, 12], [194481, 13], [117369, 14], [115101, 15], [186921, 16], [2893401, 17], [273861, 18], nil, [378189, 20], [1035909, 21], [9506889, 22], [2099601, 23], [882441, 24], [5639949, 25], [85562001, 26], [6765381, 27], [3403701, 28], nil, [3925341, 30], nil, [5420709, 32], [83908629, 33], nil, [50759541, 35], [7941969, 36], nil, [102880449, 38], nil, [15505749, 40], [170067681, 41], [35328069, 42], nil, nil, [60888429, 45], nil, nil, [36180081, 48], nil, [192341709, 50], nil, nil, nil, nil, nil, [139551741, 56], nil, nil, nil, [113834889, 60], nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil]
