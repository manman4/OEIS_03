#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A374091
#   a(n) is the smallest nonnegative integer k where exactly n ordered pairs
#   of positive integers (x, y) exist such that x^2 + 3*x*y + y^2 = k.
#
# ------------------------------------------------------------------
# 理論（判別式 +5。今までと違い「不定符号」形式）
# ------------------------------------------------------------------
#   x^2 + 3xy + y^2 の判別式は 3^2 - 4 = 5 で正、つまり不定符号形式であり、
#   対応するのは虚二次体ではなく実二次体 Q(sqrt 5) である。判別式 5 は
#   基本判別式で Z[(1+sqrt5)/2] は最大整環、類数は 1。
#   （y -> y - x の置換で -x^2 + xy + y^2 に移り、これは Q(sqrt 5) の
#     ノルム形式 x^2 + xy - y^2 の符号違いである。）
#
#   不定符号形式なので単数群は無限で、全整数解の個数は有限にならない。
#   しかし x, y > 0 に限れば x^2+3xy+y^2 >= 5xy なので x, y <= sqrt(k) となり
#   有限個で、しかも各軌道からちょうど 1 点が x, y > 0 の領域に入る。
#
#   k = 5^t * Π p_i^{e_i} * Π q_j^{f_j}
#       p_i ≡ 1, 4 (mod 5) : 分解  11, 19, 29, 31, 41, 59, 61, 71, ...
#       q_j ≡ 2, 3 (mod 5) : 惰性（2, 3, 7, 13, 17, 23, ... もここ）
#       5 : 分岐
#
#   M(k) := Π(e_i + 1)   ... f_j が全て偶数のとき
#         := 0           ... それ以外
#   （5 の指数 t は M に効かない）
#
#   このとき、x, y > 0 の順序付き解の個数は
#
#       P(k) = M(k) - [k は平方数]
#
#   （k <= 200000 で総当たりと完全一致）。これは A328151（x^2+y^2）や
#   A374090（x^2+xy+y^2）とまったく同じ形である。
#
# ------------------------------------------------------------------
# 最小化
# ------------------------------------------------------------------
#   5^t や q^(偶数) は M を変えず k を大きくするだけだが、平方性の調整には
#   使える。P(k) = n となる最小の k は次の 2 通りを比べればよい。
#
#     (A) k は非平方, M = n
#         n 偶 -> minprod(n)   （M 偶 ⇒ ある e_i が奇 ⇒ 自動的に非平方）
#         n 奇 -> M 奇 ⇒ 全 e_i 偶 ⇒ minprod(n) は必ず平方数なので
#                 非平方化が要る。惰性素数は奇数乗にすると M = 0 になり、
#                 偶数乗では平方性が壊れない。よって唯一の手段は分岐素数 5 を
#                 奇数乗で足すことで、最安は 5 倍。-> 5 * minprod(n)
#     (B) k は平方, M = n+1  （n が偶数のときのみ可能）
#         -> minprod(n+1)   （M 奇 ⇒ 全 e_i 偶 ⇒ 自動的に平方）
#
#   まとめると
#       a(n) = min( minprod(n), minprod(n+1) )   (n 偶, n >= 2)
#       a(n) = 5 * minprod(n)                    (n 奇)
#       a(0) = 0
#
#   minprod(M) := 5k±1 型素数 11, 19, 29, 31, ... のみを使って
#                 Π(e_i + 1) = M となる最小の数
#
#   姉妹列との対応（分解素数の集合と分岐素数だけが違う）:
#       A328151  x^2+y^2      p ≡ 1 (mod 4)   分岐 2  -> n 奇で 2 倍
#       A374090  x^2+xy+y^2   p ≡ 1 (mod 3)   分岐 3  -> n 奇で 3 倍
#       A374091  x^2+3xy+y^2  p ≡ ±1 (mod 5)  分岐 5  -> n 奇で 5 倍
#
# ------------------------------------------------------------------
# 系
#   11 | a(n)、ただし a(1) = 5 のみ例外（n >= 2 では両候補とも M >= 2 で
#     最小の分解素数 11 を必ず含む）。
#   5 | a(n) <=> n が奇数。そのとき a(n) = 5*m^2（m^2 = minprod(n)）。
# ------------------------------------------------------------------
#
# 使い方:
#   ruby 374091_03.rb           # a(n) が 1000 桁を超える手前まで出力
#   ruby 374091_03.rb 30        # n = 0..30 を出力（桁数制限も併用）
#   ruby 374091_03.rb --digits=200   # 桁数の上限を変更
#   ruby 374091_03.rb --known   # 総当たりで確定済みの項と照合
#   ruby 374091_03.rb --verify  # 総当たりと突き合わせて検証

module A374091
  module_function

  # 出力する a(n) の桁数上限。ちょうどこの桁数までは出力し、
  # 超えた時点で打ち切る（1000 桁は OK、1001 桁で停止）。
  MAX_DIGITS = 1000

  # ---------- 素数まわり（prime gem に依存しない） ----------

  def prime?(n)
    return false if n < 2
    return true  if n < 4
    return false if n.even?

    d = 3
    while d * d <= n
      return false if (n % d).zero?
      d += 2
    end
    true
  end

  # p ≡ 1, 4 (mod 5) な素数（5 で分解する素数）を小さい順に count 個
  def primes_split5(count)
    ps = []
    n = 11
    while ps.size < count
      ps << n if [1, 4].include?(n % 5) && prime?(n)
      n += 1
    end
    ps
  end

  # ---------- minprod ----------

  # Π(e_i + 1) = m となる最小の Π primes[i]^{e_i}
  # 指数は非増加としてよい（小さい素数に大きい指数を割り当てるのが最適）
  def minprod(m, primes = nil, idx = 0, max_exp = Float::INFINITY, memo = {})
    return 1 if m == 1

    primes ||= primes_split5(Math.log2(m).floor + 1)
    key = [m, idx, max_exp]
    cached = memo[key]
    return cached if cached
    return nil if idx >= primes.size

    best = nil
    divisors_from(m, 2).each { |d|
      e = d - 1
      next if e > max_exp

      rest = minprod(m / d, primes, idx + 1, e, memo)
      next if rest.nil?

      cand = primes[idx]**e * rest
      best = cand if best.nil? || cand < best
    }
    memo[key] = best
  end

  def divisors_from(m, lower)
    ds = []
    d = 1
    while d * d <= m
      if (m % d).zero?
        ds << d if d >= lower
        e = m / d
        ds << e if e != d && e >= lower
      end
      d += 1
    end
    ds.sort
  end

  # ---------- 本体 ----------

  def a(n)
    return 0 if n.zero?

    if n.odd?
      # M = n（奇）は必ず平方数になるので、分岐素数 5 を 1 個足して非平方化
      w = minprod(n)
      w && 5 * w
    else
      # 非平方 M = n  /  平方 M = n+1
      cands = []
      [n, n + 1].each { |m|
        v = minprod(m)
        cands << v if v
      }
      cands.min
    end
  end

  # a(n) の桁数が max_digits を超えた時点で打ち切る。
  # nmax を与えた場合はそこでも打ち切る（どちらか早いほう）。
  # 戻り値は [n, a(n)] の配列。
  def sequence(nmax = nil, max_digits = MAX_DIGITS)
    out = []
    n = 0
    loop {
      break if nmax && n > nmax

      v = a(n)
      break if v.to_s.size > max_digits

      out << [n, v]
      n += 1
    }
    out
  end

  # k <= 6*10^7 の総当たり走査で確定した項
  KNOWN = {
    0 => 0, 1 => 5, 2 => 11, 3 => 605, 4 => 209, 5 => 73205, 6 => 2299,
    7 => 8857805, 8 => 6061, 9 => 218405, 10 => 278179, 12 => 66671,
    14 => 5285401, 15 => 26427005, 16 => 187891, 18 => 1266749,
    20 => 8067191, 24 => 2066801, 26 => 36735721
  }.freeze

  def check_known
    ok = true
    KNOWN.keys.sort.each { |n|
      want = KNOWN[n]
      got = a(n)
      good = (got == want)
      ok &&= good
      puts format('%-4s n=%-3d got=%-12d known=%d', good ? 'OK' : 'NG', n, got, want)
    }
    puts ok ? "\nmatches all #{KNOWN.size} known terms." : "\nMISMATCH."
    ok
  end

  # ---------- 検証用の総当たり ----------

  # x^2 + 3*x*y + y^2 = k となる正整数の順序対の個数
  # y^2 + 3x*y + (x^2 - k) = 0 を y について解く（判別式 5x^2 + 4k）
  def count_pairs(k)
    cnt = 0
    x = 1
    while x * x + 3 * x + 1 <= k
      disc = 5 * x * x + 4 * k
      s = Integer.sqrt(disc)
      if s * s == disc
        num = -3 * x + s
        cnt += 1 if num.positive? && num.even?
      end
      x += 1
    end
    cnt
  end

  def verify(limit = 5_000_000, nmax = 40)
    warn "brute force scan up to k = #{limit} ..."
    cnt = Array.new(limit + 1, 0)
    x = 1
    while x * x + 3 * x + 1 <= limit
      y = 1
      while x * x + 3 * x * y + y * y <= limit
        cnt[x * x + 3 * x * y + y * y] += 1
        y += 1
      end
      x += 1
    end

    first = {}
    cnt.each_with_index { |c, k| first[c] ||= k }
    first[0] = 0 # k = 0 に正の組はない

    ok = true
    (0..nmax).each { |n|
      theory = a(n)
      brute  = first[n]

      if theory <= limit
        good = (brute == theory)
        puts format('%-4s n=%-4d theory=%-14d brute=%s',
                    good ? 'OK' : 'NG', n, theory, brute.inspect)
      else
        good = brute.nil?
        puts format('%-4s n=%-4d theory=%-14d (> limit, brute=%s)',
                    good ? 'skip' : 'NG', n, theory, brute.inspect)
      end
      ok &&= good
    }
    puts ok ? "\nall matched." : "\nMISMATCH FOUND."
    ok
  end
end

if __FILE__ == $PROGRAM_NAME
  if ARGV.include?('--known')
    A374091.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A374091.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    dig = ARGV.find { |t| t =~ /\A--digits=(\d+)\z/ } ? Regexp.last_match(1).to_i
                                                      : A374091::MAX_DIGITS
    cap = ARGV.find { |t| t =~ /\A\d+\z/ }&.to_i
    rows = A374091.sequence(cap, dig)
    rows.each { |n, v| puts "#{n} #{v}" }
    last = rows.last
    if last && (cap.nil? || last[0] < cap)
      nxt = last[0] + 1
      warn "stopped: a(#{nxt}) has #{A374091.a(nxt).to_s.size} digits (> #{dig})"
    end
  end
end
