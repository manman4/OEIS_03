def A(n)
  n.to_s.split('').map(&:to_i).inject(:*)
end

def B(n)
  ary = [0]
  (1..n).each{|i|
    j = i * A(i)
    ary << j if j > 0
  }
  ary.uniq.sort
end

n = 150000000
ary = B(n)
b = []
(1..ary.size - 1).each{|i|
  j = ary[i - 1]
  b << j if j + 1 == ary[i]
  # break if j >= n
  # print i
  # print ' '
  # puts j
}
p b


# def A(n)
#   n.to_s.split('').map(&:to_i).inject(:*)
# end

# def B(n)
#   (1..n).each{|i|
#     if n % i == 0
#       if A(i) == (n / i)
#         return i
#       end
#     end
#   }
#   false
# end

# ary = [0, 24, 2510, 5210, 8991, 56384, 348732, 460719, 867839, 28997919, 193889375, 254181375, 419321664, 1018179999, 2654951424, 1297015971839]
# ary.each{|i| p [i, B(i), B(i + 1)]}
