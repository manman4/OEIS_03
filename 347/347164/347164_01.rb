ary = []
cnt = 2
k = 4
mod = 10 ** 10
while cnt < 100000
  k = (k * 2) % mod
  # p k
  cnt += 1
  # kに含まれる異なる数字の個数を数える
  s = k.to_s.rjust(10, '0').chars.uniq.size
  if s == 10
    p [cnt, k]
    ary << cnt
  end
end

p ary
 
