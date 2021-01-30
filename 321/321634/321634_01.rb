k = 5
cnt = 0
[1, 2, 3].repeated_permutation(3 * k){|c|
  if (1..3 * k - 1).map{|i| c[i] - c[i - 1]}.all?{|i| i != 0}
    if c.count(1) == k && c.count(2) == k && c.count(3) == k
      cnt += 1
      p [cnt, c]
    end
  end
}
p cnt
