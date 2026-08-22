#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A399202  （関係式版）
#   a(n) is the smallest nonnegative integer k where there are exactly n
#   nonnegative integer solutions to x^2 + y^2 = k.
#
#   399202_01.rb が minprod を直接使うのに対し、こちらは証明済みの関係式
#
#       a(n) = min( A018782(n), A018782(n-1) )   （偶数 n >= 2）
#       a(n) = 2 * A018782(n)                    （奇数 n >= 3）
#       a(0) = 3,  a(1) = 0
#
#   をそのまま実装したもの。
#
# ------------------------------------------------------------------
# 範囲条件について
# ------------------------------------------------------------------
#   n = 1 に奇数用の式を当てると 2*A018782(1) = 2 となるが、実際は
#   a(1) = 0 である（m = 0 が (0, 0) の 1 解を持つ）。
#   n = 0 は偶数用の式に A018782(0) が必要になるが、A018782 の OFFSET は
#   1 なので定義域外である。よって初期値 a(0) = 3, a(1) = 0 は別扱い。
#   （R(0..3) = 1, 2, 1, 0 なので、0 解の最小は 3、1 解の最小は 0。）
#
# ------------------------------------------------------------------
# OFFSET に注意
# ------------------------------------------------------------------
#   A018782 : OFFSET 1
#       A018782(r) = D(m) = r となる最小の正整数 m   (r >= 1)
#       1, 5, 25, 65, 625, 325, 15625, 1105, 4225, 8125, ...
#       本ファイルでの引数は n と n-1 で、n >= 2 なので常に 1 以上。
#
#   A006339 : OFFSET 0
#       A006339(j) = ちょうど j 個の相異なるピタゴラス三角形を持つ
#                    最小の斜辺                        (j >= 0)
#       1, 5, 25, 125, 65, 3125, 15625, 325, 390625, ...
#       主公式では使わないが、同値な別表現
#           a(2k+1) = 2*A006339(k)^2                 (k >= 1)
#           a(2k)   = min(A018782(2k), A006339(k-1)^2) (k >= 1)
#       を a_via_a006339 として実装し、--check で主公式と相互照合する。
#       こちらは添字が「三角形の個数 j = (D-1)/2」なので k-1 が現れ、
#       k = 1 のとき A006339(0) = 1 を使う。OFFSET 1 と誤読して
#       A006339(1) = 5 を入れると a(2) = 5 となり初項から狂う。
#
# ------------------------------------------------------------------
# 補助列の計算
# ------------------------------------------------------------------
#   D(m) := Π(α_i + 1)  （p_i ≡ 1 mod 4 の指数 α_i について）
#           ただし q ≡ 3 mod 4 が奇数乗で現れれば D(m) = 0
#
#   A018782(r) = min{ Π p_i^{α_i} : Π(α_i + 1) = r }
#       2 の冪や q^(偶数) は D を変えず m を大きくするだけなので、
#       1 (mod 4) の素数 5, 13, 17, 29, ... だけを使えばよい。
#       r が奇数なら全指数が偶になるので A018782(r) は「奇数の平方数」。
#       r が偶数ならどこかの指数が奇なので非平方数。この二分が主公式の
#       「非平方数候補 / 平方数候補」にそのまま対応する。
#
#   A006339(j) = min{ Π p_i^{e_i} : Π(2 e_i + 1) = 2j + 1 }
#       斜辺 h に対し D(h^2) = Π(2 e_i + 1) であり、h の 2 の冪と
#       q ≡ 3 mod 4 の因子はこの値に効かない。特に A006339(j) は奇数で、
#       A018782(2j+1) = A006339(j)^2 が成り立つ。
#
#   どちらも「指数は小さい素数ほど大きく」としてよい（交換論法）。
#
# 使い方:
#   ruby 399202_02.rb           # n = 0..100 を出力
#   ruby 399202_02.rb 30        # n = 0..30 を出力
#   ruby 399202_02.rb --known   # 総当たりで確定済みの項と照合
#   ruby 399202_02.rb --check   # 補助列の値・恒等式・2つの表現の相互照合
#   ruby 399202_02.rb --verify  # 総当たりと突き合わせて検証

module A399202
  module_function

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

  # p ≡ 1 (mod 4) な素数を小さい順に count 個
  def primes_1mod4(count)
    ps = []
    n = 5
    while ps.size < count
      ps << n if prime?(n)
      n += 4
    end
    ps
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

  # ---------- A018782 (OFFSET 1) ----------

  # A018782(r) = Π(α_i + 1) = r となる最小の Π p_i^{α_i}
  def a018782(r, primes = nil, idx = 0, max_exp = Float::INFINITY, memo = {})
    raise ArgumentError, "A018782 has OFFSET 1 (got #{r})" if r < 1
    return 1 if r == 1

    primes ||= primes_1mod4(Math.log2(r).floor + 1)
    key = [r, idx, max_exp]
    cached = memo[key]
    return cached if cached
    return nil if idx >= primes.size

    best = nil
    divisors_from(r, 2).each { |d|
      e = d - 1
      next if e > max_exp

      rest = a018782(r / d, primes, idx + 1, e, memo)
      next if rest.nil?

      cand = primes[idx]**e * rest
      best = cand if best.nil? || cand < best
    }
    memo[key] = best
  end

  # ---------- A006339 (OFFSET 0) ----------

  # A006339(j) = Π(2*e_i + 1) = 2j+1 となる最小の Π p_i^{e_i}
  def a006339(j)
    raise ArgumentError, "A006339 has OFFSET 0 (got #{j})" if j.negative?

    least_hypotenuse(2 * j + 1)
  end

  # 奇数 m に対し Π(2*e_i + 1) = m となる最小の Π primes[i]^{e_i}
  def least_hypotenuse(m, primes = nil, idx = 0, max_exp = Float::INFINITY, memo = {})
    return 1 if m == 1

    primes ||= primes_1mod4(Math.log2(m).floor + 1)
    key = [m, idx, max_exp]
    cached = memo[key]
    return cached if cached
    return nil if idx >= primes.size

    best = nil
    divisors_from(m, 3).each { |d|
      e = (d - 1) / 2 # d は奇数なのでちょうど割り切れる
      next if e > max_exp

      rest = least_hypotenuse(m / d, primes, idx + 1, e, memo)
      next if rest.nil?

      cand = primes[idx]**e * rest
      best = cand if best.nil? || cand < best
    }
    memo[key] = best
  end

  # ---------- 本体（主公式: A018782 だけを使う） ----------

  def a(n)
    return 3 if n.zero? # 二平方和で表せない最小の非負整数
    return 0 if n == 1  # m = 0 の (0, 0)

    if n.even?
      # 非平方数候補 D(m)=n と 平方数候補 D(m)=n-1
      [a018782(n), a018782(n - 1)].min
    else
      # 平方数候補は D(m)=n-1 が偶数なので存在しない。
      # 非平方数候補は 2 * (奇数の平方数) の形で、最小は 2*A018782(n)
      2 * a018782(n)
    end
  end

  # ---------- 同値な別表現（A006339 版、相互照合用） ----------

  def a_via_a006339(n)
    return 3 if n.zero?
    return 0 if n == 1

    if n.odd?
      k = (n - 1) / 2                          # k >= 1
      2 * a006339(k)**2                        # = 2 * A018782(2k+1)
    else
      k = n / 2                                # k >= 1
      [a018782(2 * k), a006339(k - 1)**2].min  # A006339 は OFFSET 0
    end
  end

  def sequence(nmax)
    (0..nmax).map { |n| a(n) }
  end

  # ---------- 照合 ----------

  A018782_TERMS = [ # r = 1..28（OEIS 公開値）
    1, 5, 25, 65, 625, 325, 15625, 1105, 4225, 8125, 9765625, 5525,
    244140625, 203125, 105625, 27625, 152587890625, 71825, 3814697265625,
    138125, 2640625, 126953125, 2384185791015625, 160225, 17850625,
    3173828125, 1221025, 3453125
  ].freeze

  A006339_TERMS = [ # j = 0..29（OEIS 公開値）
    1, 5, 25, 125, 65, 3125, 15625, 325, 390625, 1953125, 1625, 48828125,
    4225, 1105, 6103515625, 30517578125, 40625, 21125, 3814697265625,
    203125, 95367431640625, 476837158203125, 5525, 11920928955078125,
    274625, 5078125, 1490116119384765625, 528125, 25390625,
    186264514923095703125
  ].freeze

  # k <= 3*10^6 の総当たり走査で確定した A399202 の項
  KNOWN = {
    0 => 3, 1 => 0, 2 => 1, 3 => 50, 4 => 25, 5 => 1250, 6 => 325,
    7 => 31250, 8 => 1105, 9 => 8450, 10 => 4225, 12 => 5525,
    14 => 203125, 15 => 211250, 16 => 27625, 18 => 71825
  }.freeze

  def report(label, bad)
    puts format('%-52s : %s', label, bad.empty? ? 'OK' : "NG #{bad.inspect}")
    bad.empty?
  end

  def sq?(n)
    s = Integer.sqrt(n)
    s * s == n
  end

  def check
    ok = true
    ok &= report("A018782(r), r = 1..#{A018782_TERMS.size} (OFFSET 1)",
                 (1..A018782_TERMS.size).reject { |r| a018782(r) == A018782_TERMS[r - 1] })
    ok &= report("A006339(j), j = 0..#{A006339_TERMS.size - 1} (OFFSET 0)",
                 (0..A006339_TERMS.size - 1).reject { |j| a006339(j) == A006339_TERMS[j] })
    ok &= report('A018782(2j+1) = A006339(j)^2',
                 (0..13).reject { |j| a018782(2 * j + 1) == a006339(j)**2 })
    ok &= report('A018782(奇数) は奇数の平方数',
                 (1..49).select(&:odd?).reject { |r| v = a018782(r); sq?(v) && v.odd? })
    ok &= report('A018782(偶数) は非平方数',
                 (2..50).select(&:even?).select { |r| sq?(a018782(r)) })
    ok &= report('主公式 == A006339 版 (n = 0..200)',
                 (0..200).reject { |n| a(n) == a_via_a006339(n) })
    puts ok ? "\nall consistent." : "\nINCONSISTENT."
    ok
  end

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

  # x^2 + y^2 = k となる非負整数解（順序付き）の個数
  def count_solutions(k)
    return 1 if k.zero?

    cnt = 0
    y = 0
    while y * y <= k
      r = k - y * y
      s = Integer.sqrt(r)
      cnt += 1 if s * s == r
      y += 1
    end
    cnt
  end

  def verify(limit = 5_000_000, nmax = 40)
    warn "brute force scan up to k = #{limit} ..."
    cnt = Array.new(limit + 1, 0)
    y = 0
    while y * y <= limit
      x = 0
      while x * x + y * y <= limit
        cnt[x * x + y * y] += 1
        x += 1
      end
      y += 1
    end

    first = {}
    cnt.each_with_index { |c, k| first[c] ||= k }

    ok = true
    (0..nmax).each { |n|
      theory = a(n)
      brute  = first[n]

      if theory <= limit
        good = (brute == theory)
        puts format('%-4s n=%-4d formula=%-16d brute=%s',
                    good ? 'OK' : 'NG', n, theory, brute.inspect)
      else
        good = brute.nil?
        puts format('%-4s n=%-4d formula=%-16d (> limit, brute=%s)',
                    good ? 'skip' : 'NG', n, theory, brute.inspect)
      end
      ok &&= good
    }
    puts ok ? "\nall matched." : "\nMISMATCH FOUND."
    ok
  end
end

if __FILE__ == $PROGRAM_NAME
  if ARGV.include?('--check')
    A399202.check
  elsif ARGV.include?('--known')
    A399202.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A399202.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    nmax = (ARGV[0] || 100).to_i
    A399202.sequence(nmax).each_with_index { |v, n| puts "#{n} #{v}" }
  end
end
