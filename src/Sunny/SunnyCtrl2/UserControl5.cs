﻿/********************************************************************************
*					Sunny - version 1.0.0.202101010001							*
*********************************************************************************
* Copyright (C) 2002-2020 by Tangram Team.   All Rights Reserved.				*
*
* This SOURCE CODE is governed by a BSD - style license that can be
* found in the LICENSE file.
*
* CONTACT INFORMATION:
* mailto:tangramteam@outlook.com
* https://www.tangram.dev
********************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Universe;

namespace SunnyCtrl2
{
    public partial class UserControl5 : UserControl
    {
        Xobj hostXobj = null;
        public UserControl5()
        {
            InitializeComponent();
        }

        private void UserControl5_Load(object sender, EventArgs e)
        {
            if (Cosmos.CreatingXobj != null)
            {
                hostXobj = Cosmos.CreatingXobj;
                hostXobj.OnCloudMessageReceived += HostXobj_OnCloudMessageReceived;
            }
        }

        private void HostXobj_OnCloudMessageReceived(Wormhole cloudSession)
        {
        }
    }
}
