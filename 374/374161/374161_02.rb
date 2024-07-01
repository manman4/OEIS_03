def A(n)
  m = Math.sqrt(n).to_i + 1
  h = {}
  (1..m).each{|i|
    (1..m).each{|j|
      k = i * i + 19 * j * j
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

n = 5 * 10 ** 8
ary = A(n)

# h[k] = i の 最小のk を出力
p a = (0..100).map{|i| ary.find{|k, j| j == i}}
# (0..100).each{|i|
#   print i
#   print ' '
#   puts a[i] 
# }

# [nil, [20, 1], [140, 2], [700, 3], [1540, 4], [17500, 5], [7700, 6], [122500, 7], [26180, 8], [53900, 9], [192500, 10], [7035875, 11], [130900, 12], [592900, 13], [4812500, 14], [1347500, 15], [602140, 16], [150062500, 17], [916300, 18], nil, [3272500, 20], [33687500, 21], [14822500, 22], nil, [3010700, 24], [66027500, 25], nil, [10079300, 27], [81812500, 28], nil, [22907500, 30], [370562500, 31], [15053500, 32], nil, nil, nil, [21074900, 36], nil, nil, nil, [75267500, 40], nil, nil, nil, nil, [251982500, 45], nil, nil, [105374500, 48], nil, nil, nil, nil, nil, [231823900, 54], nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil, nil]


