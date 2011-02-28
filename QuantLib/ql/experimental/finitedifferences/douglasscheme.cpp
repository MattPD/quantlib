/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2008 Andreas Gaida
 Copyright (C) 2008 Ralph Schreyer
 Copyright (C) 2008 Klaus Spanderen

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/experimental/finitedifferences/douglasscheme.hpp>

namespace QuantLib {
    DouglasScheme::DouglasScheme(
        Real theta,
        const boost::shared_ptr<FdmLinearOpComposite> & map,
        const std::vector<boost::shared_ptr<FdmDirichletBoundary> > & bcSet)
    : dt_(Null<Real>()),
      theta_(theta),
      map_(map),
      bcSet_(bcSet) {
    }

    void DouglasScheme::step(array_type& a, Time t) {
        QL_REQUIRE(t-dt_ > -1e-8, "a step towards negative time given");
        map_->setTime(std::max(0.0, t-dt_), t);
        Array y = a + dt_*map_->apply(a);
        for (Size i=0; i<bcSet_.size(); i++)
            bcSet_[i]->applyAfterApplying(y);

        for (Size i=0; i < map_->size(); ++i) {
            Array rhs = y - theta_*dt_*map_->apply_direction(i, a);
            y = map_->solve_splitting(i, rhs, -theta_*dt_);
        }
        a = y;

        for (Size i=0; i<bcSet_.size(); i++)
            bcSet_[i]->applyAfterApplying(a);
    }

    void DouglasScheme::setStep(Time dt) {
        dt_=dt;
    }
}
