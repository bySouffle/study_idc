//
// Created by bysou on 2022/4/18.
//

#include "st_station_data.h"

CStationData::CStationData(const std::string &obt_id,
                         const std::string &datetime,
                         int t,
                         int p,
                         int u,
                         int wd,
                         int wf,
                         int r,
                         int vis) : obt_id_(std::move(obt_id)), datetime_(std::move(datetime)), t_(t),
                                    p_(p), u_(u), wd_(wd), wf_(wf), r_(r), vis_(vis) {}