# If n appears so do 2n+1 and 4n+1.
def A(n)
  a = [1]
  i = 0
  while i <= n
    j = a[i]
    p [j, j * 2 + 1] if a.include?(j * 2 + 1)
    p [j, j * 4 + 1] if a.include?(j * 4 + 1)
    a << j * 2 + 1
    a << j * 4 + 1
    a.sort!.uniq!
    i += 1
  end
  a[0..n - 1]
end

n = 10000
ary = A(n)
#p ary.map{|i| i.to_s(2)}

# 2進数表記した時0が連続しないかどうか
def check_0s(n)
  n.to_s(2).match(/0{2,}/).nil?
end

def B(n)
  a = [1]
  i = 2
  while a.size < n
    if check_0s(i) && i % 2 == 1
      a << i
    end
    i += 1
  end
  a
end

ary1 = B(n)
p ary == ary1

# (1..n).each{|i|
#   j = ary[i - 1]
#   break if j.to_s.size > 1000
#   print i
#   print ' '
#   puts j
# }