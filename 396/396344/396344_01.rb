# a(0) = 0; for n > 0, a(n) = a(n-1) - n if nonnegative and not already in the sequence, otherwise a(n) = a(n-1) + n - 1.
def generate(limit)
  seq = [0]
  seen = {0 => true}

  (1..limit).each do |n|
    candidate = seq[n - 1] - n
    if candidate >= 0 && !seen[candidate]
      val = candidate
    else
      val = seq[n - 1] + n - 1
    end
    seq << val
    seen[val] = true
  end

  seq
end

limit = 100000000
seq = generate(limit)

output_seen = {}
first_n = {}
(0..limit - 2).each do |n|
  if seq[n] == seq[n + 2]
    val = seq[n]
    unless output_seen[val]
      output_seen[val] = true
      first_n[val] = n
      puts val
    end
  end
end
p output_seen.keys
p first_n.values

# 10
# 19
# 31
# 58
# 96
# 181
# 303
# 574
# 966
# 1835
# 3095
# 5886
# 9936
# 18905
# 31923
# 60750
# 102594
# 195251
# 329751
# 627578
# 1059904
# 2017213
# 3406847
# 6483942
# 10950670
# 20841435
# 35198927
# 66991006
# 113140576
# [10, 19, 31, 58, 96, 181, 303, 574, 966, 1835, 3095, 5886, 9936, 18905, 31923, 60750, 102594, 195251, 329751, 627578, 1059904, 2017213, 3406847, 6483942, 10950670, 20841435, 35198927, 66991006, 113140576]
# [5, 10, 13, 28, 39, 86, 123, 272, 393, 870, 1261, 2792, 4051, 8970, 13019, 28828, 41845, 92658, 134501, 297828, 432327, 957310, 1389635, 3077096, 4466729, 9890766, 14357493, 31792080, 46149571]


