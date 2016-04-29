/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/applications-module.h"
#include "ns3/energy-module.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Wifi0");

int main (int argc, char *argv[])
{
    uint32_t nStaNodes=2;
    double rnf=10;
    double dist=5;
    double rxg=1;

    time_t timex;
    time(&timex);
    RngSeedManager::SetSeed(timex);
    RngSeedManager::SetRun(1);

    //command line arguments
    CommandLine cmd;
    cmd.AddValue ("node", "Number of sta nodes", nStaNodes);
    cmd.AddValue ("rnf", "Rx Noise Figure", rnf);
    cmd.AddValue ("dist", "Distance", dist);
    cmd.AddValue ("rxg", "RxGain", rxg);
    cmd.Parse (argc, argv);

    //create the nodes
    NodeContainer apNodes;
    NodeContainer staNodes;
    apNodes.Create(1);
    staNodes.Create(nStaNodes);

    //create the wifi connection
    WifiHelper wifi;
    wifi.SetStandard(WIFI_PHY_STANDARD_80211b);

    NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
    //80211b standard has max 11Mbps which is chosen here
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue ("DsssRate11Mbps"),
                                "ControlMode",StringValue ("DsssRate11Mbps"));

    YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
    phy.SetErrorRateModel("ns3::YansErrorRateModel");
    phy.Set ("RxGain", DoubleValue (rxg) );
    phy.Set ("RxNoiseFigure",DoubleValue(rnf));

    //set attributes of the channel

    YansWifiChannelHelper channel;
    channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    channel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel","Exponent", DoubleValue (3.0));
    phy.SetChannel (channel.Create ());
    //install the wifi to the nodes
    Ssid ssid = Ssid ("wifi-default");
    wifiMac.SetType ("ns3::ApWifiMac","Ssid", SsidValue (ssid));
    NetDeviceContainer apDevice = wifi.Install (phy, wifiMac, apNodes);
    wifiMac.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid),"ActiveProbing", BooleanValue (false));
    NetDeviceContainer staDevice = wifi.Install (phy, wifiMac, staNodes);

    //set mobility model and position of nodes, a node is at (0,0,0) and doesn't move
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    positionAlloc->Add (Vector (0.0, 0.0, 0.0));
    MobilityHelper mobility;
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install(apNodes);
    //the sta nodes start from a fixed position and they move randomly if chosen so
    Ptr<ListPositionAllocator> positionAlloc1 = CreateObject<ListPositionAllocator> ();
    for(uint32_t cnt=1;cnt<nStaNodes+1;cnt++)
    {
        positionAlloc1->Add (Vector (0, dist, 0));
        positionAlloc1->Add (Vector (0, -dist, 0));
    }
    MobilityHelper mobility1;
    mobility1.SetPositionAllocator (positionAlloc1);
    mobility1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility1.Install(staNodes);

    //install internet stack and addresses of nodes
    InternetStackHelper istack;
    istack.Install (apNodes);
    istack.Install (staNodes);

    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer j =address.Assign (apDevice);
    Ipv4InterfaceContainer i =address.Assign (staDevice);

    //energy model. BasicEnergySource+WifiRadioEnergyModel
    BasicEnergySourceHelper basicSourceHelper;
    basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (100));
    EnergySourceContainer s1 = basicSourceHelper.Install (apNodes);
    EnergySourceContainer s2 = basicSourceHelper.Install (staNodes);

    WifiRadioEnergyModelHelper radioEnergyHelper;
    radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0.0857));
    radioEnergyHelper.Set ("RxCurrentA", DoubleValue (0.0528));
    radioEnergyHelper.Set ("IdleCurrentA", DoubleValue (0.0188));

    DeviceEnergyModelContainer d1 = radioEnergyHelper.Install (apDevice, s1);
    DeviceEnergyModelContainer d2 = radioEnergyHelper.Install (staDevice, s2);

    //install on-off application to odd nodes
    uint16_t port = 9;
    ApplicationContainer sourceApps[nStaNodes];

    for(uint32_t n=0;n<nStaNodes;n=n+2)
    {
        OnOffHelper OnOff=OnOffHelper("ns3::UdpSocketFactory", InetSocketAddress (i.GetAddress(n+1), port));
        OnOff.SetAttribute("DataRate",DataRateValue(5000000));
        OnOff.SetAttribute("PacketSize",UintegerValue(1024));
        OnOff.SetAttribute("OnTime",StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
        OnOff.SetAttribute("OffTime",StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
        sourceApps[n] = OnOff.Install (staNodes.Get (n));
        sourceApps[n].Start (Seconds (2.0));
        sourceApps[n].Stop (Seconds (10.0));

    }


    //install packet sink application to even nodes
    PacketSinkHelper sink ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps[nStaNodes];
    for(uint32_t n=1;n<nStaNodes;n=n+2)
    {
        sinkApps[n]=sink.Install (staNodes.Get (n));
        sinkApps[n].Start (Seconds (2.0));
        sinkApps[n].Stop (Seconds (10.0));
    }
    //make the routing table
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    //create the file to store the results
    std::ofstream myfile;
    myfile.open ("dist.txt",std::ios_base::app);


    //Flow Monitor is used to measure the throghput and the lost packets
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    Simulator::Stop (Seconds (10.0));
    Simulator::Run ();

    double throughput;

    //flow monitor for the IP packets
    monitor->CheckForLostPackets ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
        std::cout << "Flow: " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
        throughput=i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1000000;
        std::cout << "Distance: " <<  dist << "m, Throughput: " <<  throughput << " Mbps\n";
        myfile << dist <<"\t"<<rnf<<"\t"<<rxg<<"\t"<<throughput<<"\n";
    }
    Simulator::Destroy ();
    myfile.close();
    return 0;
}
