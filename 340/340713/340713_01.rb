require 'prime'

def power0(a, n)
  return 1 if n == 0
  k = power0(a, n >> 1)
  k *= k
  return k if n & 1 == 0
  return k * a
end

# x > 0
def sigma(x, i)
  sum = 1
  pq = i.prime_division
  pq.each{|a, n| sum *= (power0(a, (n + 1) * x) - 1) / (power0(a, x) - 1)}
  sum
end

f = File.open("b058072.txt")
str = f.read  # 全て読み込む
f.close

cnt = 0
str.split(/\R/).each{|i|
  m = i.split(' ')[-1].to_i
  if sigma(1, m + 1) == 4 * sigma(1, m)
    cnt += 1
    print cnt
    print " "
    puts m
  end
}

