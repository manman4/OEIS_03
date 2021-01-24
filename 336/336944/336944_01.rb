def A(n)
  n.to_s.split('').map(&:to_i).inject(:*)
end

def B(n)
  ary = [0, 0]
  (1..n).each{|i|
    j = i * A(i)
    ary << j if j > 0
  }
  # ary.group_by(&:itself).select{|k, v| v.size > 1}.keys.sort
  ary.group_by(&:itself).select{|k, v| v.size > 1}.values.sort.select{|i| i[0] <= n}
end

n = 50000000
ary = B(n)
# (1..500).each{|i|
#   j = ary[i - 1]
#   break if j >= n
#   print i
#   print ' '
#   puts j
# }


def C(n)
  n.to_s.split('').map(&:to_i).inject(:*)
end

def D(n)
  ary = []
  (1..n).each{|i|
    if n % i == 0
      if C(i) == (n / i)
        ary << i
      end
    end
  }
  ary
end

a = ary.map{|i| D(i[0])}
(1..500).each{|i|
  j = a[i - 1]
  # break if j >= n
  print i
  print ' '
  print j
  puts
}