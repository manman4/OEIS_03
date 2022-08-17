def A000111(n)
  e_ary = [1]
  ary = [1]
  n.times{|i|
    new_ary = ary.unshift(0)
    (1..i + 1).each{|i| new_ary[i] += new_ary[i - 1]}
    ary = new_ary.reverse
    e_ary << ary[0]
  }
  e_ary
end

# A(0)=0
def A(n)
  ary = A000111(2 * n - 1)
  [0] + (1..n).map{|i| ary[2 * i - 1]}
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A354520(n)
  a = A(n)
  ary = [0, 0]
  (2..n).each{|i|
    if i % 2 == 0
      ary << ary[-1] + 1
    else
      ary << (1..i / 2).inject(0){|s, j| s + (-1) ** (j + 1) * a[j] * ncr(i, 2 * j)}
    end
  }
  ary
end

n = 500
ary = A354520(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
